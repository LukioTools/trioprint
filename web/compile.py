
import re;
import gzip;


s = ""
with open('index.html', 'r') as f:
    with open('index.css', 'r') as css:
        s = f.read().replace('<link rel="stylesheet" href="index.css">', "<style>"+css.read().replace('\n', "").replace(' ', '')+"</style>")
with open('compiled.html', 'w+') as um:
    um.write(s)
with gzip.open('compiled.html.gz', 'wb') as f:
    f.write(s.encode('utf-8'))
s = re.sub(r"//.*", '', s)
s = re.sub(r"/\*[\s\S]*?\*/", '', s)
s = re.sub(r"<!--[\s\S]*?-->", '', s)
s = re.sub(r"\n", '', s)
#some spaces are important, so just gets rid of subsequent spaces, just hope it doesnt fuck up your code :3
s = re.sub(r" +", ' ', s)
print(s)
with open('compiled-minified.html', 'w+') as o:
    o.write(s)

with gzip.open('compiled-minified.html.gz', 'wb') as f:
    f.write(s.encode('utf-8'))



print("finished")
    
