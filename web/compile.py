
import re;
with open('compiled.html', 'w+') as o:
    with open('index.css', 'r') as css:
        with open('index.html', 'r') as f:
            s = f.read().replace('<link rel="stylesheet" href="index.css">', "<style>"+css.read().replace('\n', "").replace(' ', '')+"</style>")
            s = re.sub(r"//.*", '', s)
            s = re.sub(r"/\*[\s\S]*?\*/", '', s)
            s = re.sub(r"<!--[\s\S]*?-->", '', s)
            s = re.sub(r"\n", '', s)
            #some spaces are important, so just gets rid of subsequent spaces, just hope it doesnt fuck up your code :3
            s = re.sub(r" +", ' ', s)
            print(s)
            o.write(s);


print("finished");
    
