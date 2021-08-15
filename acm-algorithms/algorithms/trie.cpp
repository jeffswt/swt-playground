
class Trie
{
public:
    int ncnt, root;
    int ch[maxn][26];
    bool flag[maxn];
    int size[maxn], data[maxn];
    int make_node(void)
    {
        int p = ++ncnt;
        memclr(ch[p], 25);
        flag[p] = false;
        size[p] = data[p] = 0;
        return p;
    }
    void insert(char str[], int vdata = 0)
    {
        int p = root;
        for (int i = 0; str[i] != '\0'; i++) {
            int v = str[i] - 'a';
            size[p] += 1;
            if (!ch[p][v])
                ch[p][v] = make_node();
            p = ch[p][v];
        }
        size[p] += 1;
        flag[p] = true;
        data[p] = vdata;
        return ;
    }
    void remove(char str[])
    {
        int p = root;
        for (int i = 0; str[i] != '\0'; i++) {
            int v = str[i] - 'a';
            size[p] -= 1;
            p = ch[p][v];
        }
        size[p] -= 1;
        flag[p] = false;
        return ;
    }
    bool find(char str[])
    {
        int p = root;
        for (int i = 0; str[i] != '\0'; i++) {
            int v = str[i] - 'a';
            if (!ch[p][v])
                return false;
            p = ch[p][v];
        }
        // return anything you want here
        return false;
    }
    void init(void)
    {
        ncnt = 0;
        root = make_node();
        return ;
    }
} trie;
