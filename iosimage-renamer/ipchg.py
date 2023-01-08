
import argparse
import datetime
import os
import re
import shutil
import sys


def orig_fn(fn: str) -> str:
    """Retrieve original filename from image filename [fn], which should be in
    the form `nnnn YYYY-mm-dd [IMG_nnnn.EXT].ext`, whereas the substring
    enclosed in the square brackets is the original filename. If such an
    original filename is not found, the original filename will be returned.

    @note The filename [fn] should not include parent directories."""

    return re.sub(r'^.*?\[(.*?)\].*?$', r'\1', fn)


def exif_date(t: datetime.datetime, fp: str) -> datetime.datetime:
    """Retrieves a file path [fp], read its EXIF data (iff is *.HEIC image),
    and return its min with a given timestamp [t]. [t] itself will be returned
    verbatim if EXIF cannot be read from the HEIC image or is not HEIC."""

    if not fp.lower().endswith('heic'):
        return t
    # platform dependent
    if sys.platform == 'linux':
        import piexif
        import pyheif
        # read heic file
        try:
            heic_file = pyheif.read(fp)
        except Exception as err:
            print(err)
            return t
        # extract exif from heic
        exif = [i['data'] for i in heic_file.metadata if i['type'] == 'Exif']
        if len(exif) <= 0:
            return t
        exif = exif[0]
        # convert raw exif to exif dict
        try:
            ed = piexif.load(exif)
        except Exception as err:
            print(err)
            return t
        # extract timestamp
        dt_b = ed.get('0th', {}).get(306, None)
        if not dt_b:
            return t
        # parse datetime
        try:
            dt_s = dt_b.decode('utf8', 'ignore')
            dt = datetime.datetime.strptime(dt_s, '%Y:%m:%d %H:%M:%S')
        except Exception as err:
            print(err)
            return t
        # finish
        return min(t, dt)
    else:
        print('Warning: HEIC-EXIF tools are not installed')
    return t


def convert_dir(root_path: str,
                revert_filename=False,
                run_format=False,
                preview_mode=True,
                ) -> None:
    """Transform all files in the dir with a given config."""

    for group_dirname in os.listdir(root_path):
        for filename in os.listdir(f'{root_path}/{group_dirname}'):
            file_path = f'{root_path}/{group_dirname}/{filename}'
            if not os.path.isfile(file_path):
                continue
            target_filename = filename

            if revert_filename:
                # get its original filename only
                orig_filename = orig_fn(filename)
                target_filename = orig_filename
            elif run_format:
                # convert file to `nnnn YYYY-mm-dd [IMG_nnnn.EXT].ext`
                orig_filename = orig_fn(filename)
                extension = os.path.splitext(filename)[1][1:]
                ts_modify = datetime.datetime.fromtimestamp(os.path.getmtime(file_path))
                ts_create = datetime.datetime.fromtimestamp(os.path.getctime(file_path))
                ts = min(ts_modify, ts_create)
                ts = exif_date(ts, file_path)
                ts_dt = ts.strftime('%Y-%m-%d')
                # get numeric order from original filename
                print(filename)
                img_ord_i = re.findall(r'[A-Z_](\d{4})[ .]', orig_filename)[0]
                img_ord = f'{int(img_ord_i)}'.rjust(4, '0')
                ts_display = ts.strftime('%Y-%m-%d')
                target_filename = f'{img_ord} {ts_display} [{orig_filename}].{extension.lower()}'

            # execute transformation
            if not preview_mode:
                target_file_path = f'{root_path}/{group_dirname}/{target_filename}'
                shutil.move(file_path, target_file_path)
            else:
                print(f'"{group_dirname}/{filename}" => "{target_filename}"')
        pass
    return


def main() -> None:
    parser = argparse.ArgumentParser(
        prog='ios-img-renamer',
        description='Format (HEIC) image filenames exported from iOS')
    parser.add_argument('-d', '--directory', action='store', required=True,
                        dest='directory', type=str)
    parser.add_argument('--revert-filename', action='store_true',
                        dest='revert_filename')
    parser.add_argument('--run-format', action='store_true',
                        dest='run_format')
    parser.add_argument('--commit', action='store_true',
                        dest='commit')
    args = parser.parse_args()
    convert_dir(
        args.directory,
        revert_filename=args.revert_filename,
        run_format=args.run_format,
        preview_mode=not args.commit,
    )
    return


if __name__ == '__main__':
    main()
