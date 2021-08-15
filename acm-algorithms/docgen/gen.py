
import os
import re
import yaml


def get_module_list():
    lst = list(filter(lambda a: re.findall(r'\.md$', a) != [],
                      os.listdir('./algorithms/')))
    lst = list(map(lambda a: re.sub(r'\.md', r'', a), lst))
    return lst


def split_components(fdata):
    last_title = ''
    res = {}
    for line in fdata.split('\n'):
        if re.match(r'^# ', line):
            last_title = re.sub(r'^# (.*)$', r'\1', line)
            res[last_title] = []
        elif last_title != '':
            res[last_title].append(line)
    for title in res:
        res[title] = '\n'.join(res[title]).strip()
    return res


def process_bullet_list(cont):
    cont = cont.split('\n')
    dists = set()
    for line in cont:
        d = len(line) - len(line.lstrip(' '))
        dists.add(d)
    dists = list(sorted(list(dists)))
    d2l = dict((j, i) for (i, j) in enumerate(dists))
    label = [''] * len(dists)
    res = {}
    for line in cont:
        d = len(line) - len(line.lstrip(' '))
        l = d2l[d]
        if l + 1 == len(label):
            cur = re.sub(r'^ *\* +(.*?):.*$', r'\1', line)
        else:
            cur = re.sub(r'^ *\* +(.*?):$', r'\1', line)
        label[l] = cur
        if l + 1 == len(label):
            value = re.sub(r'^ *\* +.*?: *(.*?)$', r'\1', line)
            if label[0] not in res:
                res[label[0]] = {}
            res[label[0]][label[1]] = value
    return res


def convert_complexity(bullet):
    cols = list(sorted(list(set(sum((list(j for j in bullet[i])
                            for i in bullet), [])))))
    rows = list(sorted(list(set(i for i in bullet))))
    res = ('|     |' + ''.join(' %s |' % s for s in cols) + '\n' +
           '|-----' * (1 + len(cols)) + '|\n' +
           '\n'.join(('| %s |' % i + ''.join(
                     ' %s |' % bullet[i][j] for j in cols)) for i in rows))
    return res.strip()


def convert_invocation(invc):
    match_rule = [
        (r'^typedef (.*?)()$',
         lambda a, _: '\\texttt{typedef}\\ \\texttt{%s}' % (a)),
        (r'^(.*?) operator \(\) \((.*?)\)$',
         lambda a, b: '(%s) \\rightarrow \\texttt{%s}' % (
             ',\\ '.join('%s: \\texttt{%s}' % (
                 i.strip().split(' ')[1].strip().replace('_', '\\_'),
                 i.strip().split(' ')[0].strip()
             ) for i in b.split(',')), a
         )),
        (r'^void (.*?)\(\)()$',
         lambda a, _: '\\textrm{%s}()' % (a)),
        (r'^void (.*?)\((.+?)\)$',
         lambda a, b: '\\textrm{%s}(%s)' % (
             a, ',\\ '.join('%s: \\texttt{%s}' % (
                 i.strip().split(' ')[1].strip().replace('_', '\\_'),
                 i.strip().split(' ')[0].strip()
             ) for i in b.split(','))
         )),
        (r'^(.*?) (.*?)\(\)$',
         lambda a, b: '\\textrm{%s}() \\rightarrow \\texttt{%s}' % (b, a)),
        (r'^(.*?) (.*?)\((.+?)\)$',
         lambda a, b, c: '\\textrm{%s}(%s) \\rightarrow \\texttt{%s}' % (
             b, ',\\ '.join('%s: \\texttt{%s}' % (
                 i.strip().split(' ')[1].strip().replace('_', '\\_'),
                 i.strip().split(' ')[0].strip()
             ) for i in c.split(',')), a
         )),
        (r'^(.*?) (.*?) ([+\-*/]) (.*?) (.*?) = (.*?)$',
         lambda a, b, c, d, e, f:
             '%s: \\texttt{%s} %s %s: \\texttt{%s} \\rightarrow \\texttt{%s}' %
         (b, a, c, e, d, f)),
        (r'^(.*?) (.*?) ([+\-*/]=) (.*?) (.*?)$',
         lambda a, b, c, d, e:
             '%s: \\texttt{%s} %s %s: \\texttt{%s}' %
         (b, a, c, e, d)),
        (r'^(.*?) (.*?)$',
         lambda a, b: '%s: \\texttt{%s}' % (b.replace('_', '\\_'), a)),
    ]
    invc = invc.split('\n')
    colon = '：'
    res = []
    for line in invc:
        if not line.lstrip().startswith('* `'):
            continue
        cd, *desc = line.split(colon)
        cdl, cd, cdr = cd.split('`')
        desc = colon.join(desc)
        found = False
        for rs, rt in match_rule:
            if re.findall(rs, cd) != []:
                cd = rt(*re.findall(rs, cd)[0])
                found = True
                break
        if not found:
            print('Error while parsing line:')
            print('  unable to find corresponding rule')
            print('  ' + line)
        res.append('%s$%s$%s%s%s' % (cdl, cd, cdr, colon, desc))
    return '\n'.join(res).strip()


def process_module(mod_name):
    with open('./algorithms/%s.md' % mod_name, 'r', encoding='utf-8') as f:
        fdata = f.read()
    with open('./algorithms/%s.cpp' % mod_name, 'r', encoding='utf-8') as f:
        cdata = f.read().strip()
    fdata = fdata.split('---\n')
    header = yaml.load(fdata[1], Loader=yaml.SafeLoader)
    fdata = split_components('---\n'.join(fdata[2:]))
    comptable = process_bullet_list(fdata['Complexity'])
    fdata['Complexity'] = convert_complexity(comptable)
    fdata['Invocation'] = convert_invocation(fdata['Invocation'])
    template = '\n'.join([
        '', '# %s', '',
        '* 模板题：%s', '* 依赖：%s', '',
        '## 描述', '', '%s', '',
        '## 复杂度', '', '%s', '',
        '## 算法', '', '%s', '',
        '## 调用', '', '%s', '',
        '## 代码', '', '```cpp', '%s', '```', '',
    ]) % (header['title'], ', '.join(header['problems']),
          ', '.join(header['depends']),
          fdata['Target'], fdata['Complexity'], fdata['Solution'],
          fdata['Invocation'], cdata)
    return template


def main():
    fout = []
    for md in get_module_list():
        print('processing module "%s"...' % md)
        fout.append(process_module(md))
    fout = '\n\n'.join(fout)
    print('saving markdown file...')
    with open('out.md', 'w', encoding='utf-8') as f:
        f.write(fout)
    return

main()
