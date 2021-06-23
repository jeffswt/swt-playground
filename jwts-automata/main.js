
var elems = document.getElementById('iframename').contentDocument.getElementsByTagName('select');
for (var i = 0; i < elems.length; i++) {
    var elem = elems[i];
    elem.value = 1;
}
