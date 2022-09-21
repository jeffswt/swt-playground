# python3 -m pip install clipboard_monitor
# python3 -m pip install deep_translator
# python3 -m pip install ./pywin32-304.0-cp310-cp310-win_amd64.whl

import clipboard_monitor
from deep_translator import GoogleTranslator
import json
import os
# from PIL import Image, ImageGrab
from typing import Dict, Tuple


translate_config = {
    'source_lang': 'en',
    'target_lang': 'ja',
    'log_file': './translated.log',
    # 'proxy': 'http://localhost:12345',
}
translate_cache: Dict[Tuple[str, str, str], str] = {}


def load_translate_cache():
    if not os.path.exists(translate_config['log_file']):
        return
    with open(translate_config['log_file'], 'r', encoding='utf-8') as f:
        for line in f.readlines():
            if len(line) <= 2:
                continue
            item = json.loads(line)
            src_lang = item['src_lang']
            src_text = item['src_text']
            dst_lang = item['dst_lang']
            dst_text = item['dst_text']
            translate_cache[(src_lang, src_text, dst_lang)] = dst_text
        pass
    return


def handle_anything(*args, **kwargs):
    pass


def handle_text(text: str):
    # get source text
    to_translate = text
    print(f'---------------------------')
    print(f'{text}')
    print(f' -  -  -  -  -  -  -  -  -')
    # perform translation
    src_lang = translate_config['source_lang']
    dst_lang = translate_config['target_lang']
    if (src_lang, to_translate, dst_lang) in translate_cache:
        from_cache = True
        translated = translate_cache[(src_lang, to_translate, dst_lang)]
    else:
        from_cache = False
        translator = GoogleTranslator(
            source=src_lang,
            target=dst_lang,
            proxies={
                'http': translate_config['proxy'],
                'https': translate_config['proxy'],
            } if translate_config.get('proxy', None) is not None else {},
        )
        translated = translator.translate(to_translate)
    # get target text
    print(f'{translated}')
    print(f'---------------------------')
    print('')
    # log result
    with open(translate_config['log_file'], 'a', encoding='utf-8') as f:
        item = {
            'src_lang': src_lang,
            'src_text': to_translate,
            'dst_lang': dst_lang,
            'dst_text': translated,
        }
        item = json.dumps(item, ensure_ascii=False)
        if not from_cache:
            translate_cache[(src_lang, to_translate, dst_lang)] = translated
            f.write(f'{item}\n')
    return


def handle_files(files):
    pass


def handle_image():
    pass


load_translate_cache()
clipboard_monitor.on_update(handle_anything)
clipboard_monitor.on_text(handle_text)
clipboard_monitor.on_files(handle_files)
clipboard_monitor.on_image(handle_image)

print('Loaded.')
clipboard_monitor.wait()

###############################################################################
#   miscellany

languages = {
    'afrikaans': 'af',
    'albanian': 'sq',
    'amharic': 'am',
    'arabic': 'ar',
    'armenian': 'hy',
    'azerbaijani': 'az',
    'basque': 'eu',
    'belarusian': 'be',
    'bengali': 'bn',
    'bosnian': 'bs',
    'bulgarian': 'bg',
    'catalan': 'ca',
    'cebuano': 'ceb',
    'chichewa': 'ny',
    'chinese (simplified)': 'zh-CN',
    'chinese (traditional)': 'zh-TW',
    'corsican': 'co',
    'croatian': 'hr',
    'czech': 'cs',
    'danish': 'da',
    'dutch': 'nl',
    'english': 'en',
    'esperanto': 'eo',
    'estonian': 'et',
    'filipino': 'tl',
    'finnish': 'fi',
    'french': 'fr',
    'frisian': 'fy',
    'galician': 'gl',
    'georgian': 'ka',
    'german': 'de',
    'greek': 'el',
    'gujarati': 'gu',
    'haitian creole': 'ht',
    'hausa': 'ha',
    'hawaiian': 'haw',
    'hebrew': 'iw',
    'hindi': 'hi',
    'hmong': 'hmn',
    'hungarian': 'hu',
    'icelandic': 'is',
    'igbo': 'ig',
    'indonesian': 'id',
    'irish': 'ga',
    'italian': 'it',
    'japanese': 'ja',
    'javanese': 'jw',
    'kannada': 'kn',
    'kazakh': 'kk',
    'khmer': 'km',
    'kinyarwanda': 'rw',
    'korean': 'ko',
    'kurdish': 'ku',
    'kyrgyz': 'ky',
    'lao': 'lo',
    'latin': 'la',
    'latvian': 'lv',
    'lithuanian': 'lt',
    'luxembourgish': 'lb',
    'macedonian': 'mk',
    'malagasy': 'mg',
    'malay': 'ms',
    'malayalam': 'ml',
    'maltese': 'mt',
    'maori': 'mi',
    'marathi': 'mr',
    'mongolian': 'mn',
    'myanmar': 'my',
    'nepali': 'ne',
    'norwegian': 'no',
    'odia': 'or',
    'pashto': 'ps',
    'persian': 'fa',
    'polish': 'pl',
    'portuguese': 'pt',
    'punjabi': 'pa',
    'romanian': 'ro',
    'russian': 'ru',
    'samoan': 'sm',
    'scots gaelic': 'gd',
    'serbian': 'sr',
    'sesotho': 'st',
    'shona': 'sn',
    'sindhi': 'sd',
    'sinhala': 'si',
    'slovak': 'sk',
    'slovenian': 'sl',
    'somali': 'so',
    'spanish': 'es',
    'sundanese': 'su',
    'swahili': 'sw',
    'swedish': 'sv',
    'tajik': 'tg',
    'tamil': 'ta',
    'tatar': 'tt',
    'telugu': 'te',
    'thai': 'th',
    'turkish': 'tr',
    'turkmen': 'tk',
    'ukrainian': 'uk',
    'urdu': 'ur',
    'uyghur': 'ug',
    'uzbek': 'uz',
    'vietnamese': 'vi',
    'welsh': 'cy',
    'xhosa': 'xh',
    'yiddish': 'yi',
    'yoruba': 'yo',
    'zulu': 'zu',
}
