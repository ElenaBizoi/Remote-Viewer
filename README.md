# Remote Viewer

Remote Viewer is a web application that transform local images.
To use the application you must have the image data written in a binary file, and the size of the image written in a text file in that order: width, heigth, and number of bytes used for storage the information about one pixel.

    Installation
Clone or Download the projet using the web URL: "https://github.com/ElenaBizoi/Remote-Viewer.git or "https://svn.ctbav.ro/svn/SIP/Remote-Viewer".
Open the file "image-web" from Command Prompt and start the server giving a number for the port of the web application, between 1024 and 65535. Example: "python server.py 2555".
After the server starts to run, open in your browser "http://localhost:2555/".

    Requirements
Visual Studio 2017, C++11, Python 3.6, Node.js