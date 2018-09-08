# Simple HTTP client written in C++

## Usage

```nohighlight 
webclient http://www.fit.vutbr.cz
# downloads the default page and saves it into the file index.html in current working directory

webclient http://www.fit.vutbr.cz:80/common/img/fit_logo_cz.gif
# downloads the picture fit_logo_cz.gif and saves it into current working directory

webclient http://www.fit.vutbr.cz/study/courses/IPK/public/some\ text.txt
# downloads the file some text.txt and saves it into current working directory

webclient http://www.fit.vutbr.cz/study/courses/IPK/public/test/redir.php
# downloads and saves the file into which this link redirects

```

