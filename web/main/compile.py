
import re;
import gzip;

# this script is used to compress the main html files into gzip which you can upload to sd card.

s = ""
with open('index.html', 'r') as f:
    with open('index.css', 'r') as css:
        s = f.read().replace('<link rel="stylesheet" href="index.css">', "<style>"+css.read()+"</style>")
    with open('progress_bar.css', 'r') as pb:
        s = s.replace('<link rel="stylesheet" href="progress_bar.css">', "<style>"+pb.read()+"</style>")

with open('compiled.html', 'w+') as um:
    um.write(s)
with gzip.open('compiled-large.html.gz', 'wb') as f:
    f.write(s.encode('utf-8'))
s = re.sub(r"//.*", '', s)
s = re.sub(r"/\*[\s\S]*?\*/", '', s)
s = re.sub(r"<!--[\s\S]*?-->", '', s)
s = re.sub(r"\n", '', s)
#some spaces are important, so just gets rid of subsequent spaces, just hope it doesnt fuck up your code :3
s = re.sub(r" +", ' ', s)
print(s)
with open('compiled.html', 'w+') as o:
    o.write(s)

with gzip.open('compiled.html.gz', 'wb') as f:
    f.write(s.encode('utf-8'))



print("finished")
    
