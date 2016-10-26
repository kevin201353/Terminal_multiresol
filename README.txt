1.所使用的库： gtk, ovirt-engine Api, libcurl, minixml 2.使用C 调试Gtk库开发界面， 使用Linux ubuntu 发行版进行开发, gtk 官网： http://www.gtk.org/ ， 本系统使用GTK3.xx的版本， 因GTK3.XX 的版本与 GTK2.XX 版本不兼容。在使用时，请确保您的机器上装的是GTK3.XX的版本。 ubuntu 上安装gtk开发库的安装 $sudo apt-get install libgtk3-dev 或者 $sudo apt-get install libgtk3*

3.使用C 语言来调用ovirt-engine Api , 使用的库函数：libcurl 一般默认的linux 发行版上已安装有curl， 在使用libcurl库时，如果提示找不到, 请收到安装libcurl-devel 库文件。 有关libcurl的详细描述见官网： https://curl.haxx.se/

4.libcurl 使用http 发送 ovirt-engine api 请求， 如使用header 中使用Application/xml , 一般默认是这种格式，ovirt-engine 换回为xml格式的结果。 5.C 采用minixml 来解析xml文件。minixml 的安装可以见官网：http://www.msweet.org/
