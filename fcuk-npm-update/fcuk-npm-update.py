
import json
import os
import re
import shutil


def fcuk_npm_update() -> None:
    """ Executes the whole stuff. """
    ###########################################################################
    #  Stage 1
    print('STAGE 1: forcing latest version in package.json...')
    # read current package info
    pkgs = {}
    with open('./package.json', 'r', encoding='utf-8') as f:
        pkgs = json.loads(f.read())
    # overwrite all versions to *
    deps_keys = ['dependencies', 'devDependencies']
    for key in deps_keys:
        for i in pkgs[key]:
            pkgs[key][i] = '*'
    # write package info
    with open('./package.json', 'w', encoding='utf-8') as f:
        f.write(json.dumps(pkgs, indent=2))
    ###########################################################################
    #  Stage 2
    print('STAGE 2: installing packages...')
    shutil.rmtree('./node_modules', ignore_errors=True)
    if os.path.exists('./package-lock.json'):
        os.remove('./package-lock.json')
    os.system('npm install')
    os.system('npm update --force')
    os.system('npm audit fix')
    ###########################################################################
    #  Stage 3
    print('STAGE 3: updating package.json...')
    # resolves current dep versions.
    cur_dep_vers = {}
    with open('./package-lock.json', 'r', encoding='utf-8') as f:
        j = json.loads(f.read())
        for pkg, obj in j['packages'].items():
            if pkg.startswith('node_modules/'):
                pkg = re.sub(r'^node_modules/', r'', pkg)
                ver = obj['version']
                cur_dep_vers[pkg] = ver
    # read current package info
    with open('./package.json', 'r', encoding='utf-8') as f:
        pkgs = json.loads(f.read())
    # replace deps
    deps_keys = ['dependencies', 'devDependencies']
    for key in deps_keys:
        for i in pkgs[key]:
            pkgs[key][i] = f'^{cur_dep_vers[i]}'
    # write package info
    with open('./package.json', 'w', encoding='utf-8') as f:
        f.write(json.dumps(pkgs, indent=2) + '\n')
    ###########################################################################
    #  Stage 4
    print('STAGE 4: double check...')
    shutil.rmtree('./node_modules', ignore_errors=True)
    os.system('npm install')
    ###########################################################################
    print('ALL DONE.')
    return


if __name__ == '__main__':
    print('[F**K `NPM UPDATE`]')
    print('it never does what i want it to do.')
    print('we will update all version hinting in package.json to the latest.')
    print('you will have your old stuff deleted:')
    print('  - /node_modules/*')
    print('  - /package-lock.json')
    print('  - /package.json')
    print('and all dependencies updated to the latest version:')
    print('  - /node_modules/*')
    print('  - /package-lock.json')
    print('  - /package.json')
    # proceed = input('proceed? (y/n) ')
    proceed = True
    if proceed:
        fcuk_npm_update()
    pass
