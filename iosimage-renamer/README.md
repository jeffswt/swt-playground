
# iOS Image Renamer

Rename (HEIC) images exported from iOS (into your Windows/Linux PC) into something that contains the 'Date Taken' field.

The images must be arranged in flat subdirectories under the specified directory, i.e. paths to the images must follow the pattern `root_of_conversion/some_dir/Image.EXT`.

## Usage

```bash
python3 -m pip install -r requirements.txt
# preview the changes and ensure the script works before committing changes
python3 ipchg.py --directory [root_of_conversion] --run-format
# commit changes
python3 ipchg.py --directory [root_of_conversion] --run-format --commit
# you may change the results back to what they were at any time
python3 ipchg.py --directory [root_of_conversion] --revert-filename
python3 ipchg.py --directory [root_of_conversion] --revert-filename --commit
```

## Trivia

We don't currently support this script on Windows due to the load of nuisance required to get libheic working. Using WSL just solves the problem.
