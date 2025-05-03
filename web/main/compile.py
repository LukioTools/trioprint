
'''
 Copyright (c) <2025> <Vili Kervinen>
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

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
    
