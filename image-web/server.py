#!/usr/bin/env python
 
from http.server import BaseHTTPRequestHandler, HTTPServer
import os
import glob
import json
import base64
from ctypes import *

class ImageHandler(BaseHTTPRequestHandler):
    def serve_file(self, filename):
        filepath = os.curdir + os.sep + filename

        if not os.path.isfile(filepath):
            self.send_response(404)
            self.end_headers()
            return

        self.send_response(200)
        if filename.endswith('.html'):
            self.send_header('Content-type', 'text/html')
        else:
            self.send_header('Content-type', 'application/octet-stream')
        self.end_headers()
        
        f = open(filepath, 'rb')
        self.wfile.write(f.read())
        f.close()

    def send_error(self):
        self.send_response(500)
        self.end_headers() 

    def do_GET(self):
        filename = self.path
        if filename == '/':
            filename = "index.html"
        self.serve_file(filename)

    def do_HEAD(self):
        self.send_response(200)
        self.end_headers()

    def do_POST(self):
        length = int(self.headers['Content-Length'])
        data = self.rfile.read(length)
        request = json.loads(data.decode())
        image_data = base64.b64decode(request['data'])
        
        transform_matrix_length = 4
        if (len(request['transformMatrix']) != transform_matrix_length):
            print("Error: The received transform matrix has an invalid size")
            self.send_error()
            return

        tmp_dir = "./images"
        if not os.path.exists(tmp_dir):
	        os.makedirs(tmp_dir)
        #save image header file
        header_file = open(tmp_dir + os.sep + 'image.txt', 'w')
        header_file.write('%d\n' % request['width'])
        header_file.write('%d\n' % request['height'] + '')
        header_file.write('%d\n' % request['bytesPerPixel'] + '')
        header_file.close()

        #save image binary file
        binary = open(tmp_dir + os.sep + 'image.bin', 'wb')
        binary.write(image_data)
        binary.close()

        dll = windll.LoadLibrary("Image.dll")
        dll.transformImage.argtypes = [c_char_p, c_char_p, c_char_p, POINTER(c_int), c_int]
        transform_matrix = (c_int * transform_matrix_length)()
        for i in range(transform_matrix_length):
            transform_matrix[i] = request['transformMatrix'][i];
        p_transform_matrix = cast(transform_matrix, POINTER(c_int))
        dll.transformImage(b"images/image.txt", b"images/image.bin", b"_transformed", p_transform_matrix, 4)

        #build the response containing the transformed image
        header_file = open(tmp_dir + os.sep + 'image_transformed.txt', 'r')
        response = {}
        try:
            response['width'] = int(header_file.readline()) 
            response['height'] = int(header_file.readline()) 
            response['bytesPerPixel'] = int(header_file.readline())
        except:
            print("Error in header file")
            self.send_error()
            return
        header_file.close()

        binary_file = open(tmp_dir + os.sep + 'image_transformed.bin', 'rb')
        response['data'] = base64.b64encode(binary_file.read()).decode()
        binary_file.close()
        
        for file in glob.glob(tmp_dir + '/*'):
            os.remove(file)
        
        #send the response  
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()
        self.wfile.write(json.dumps(response).encode())
   
def run(port=80):
    socket = ('127.0.0.1', port)
    httpd = HTTPServer(socket, ImageHandler)
    print('Starting httpd...')
    httpd.serve_forever()

if __name__ == "__main__":
    from sys import argv

    if len(argv) == 2:
        run(port=int(argv[1]))
    else:
        run()
