#!/usr/bin/env python3
# python3 update of https://gist.github.com/dergachev/7028596
# Create a basic certificate using openssl: 
#     openssl req -new -x509 -keyout server.pem -out server.pem -days 365 -nodes
# Or to set CN, SAN and/or create a cert signed by your own root CA: https://thegreycorner.com/pentesting_stuff/writeups/selfsignedcert.html

import http.server
import ssl

print("Go to: https://localhost/index.html")
httpd = http.server.HTTPServer(('0.0.0.0', 443), http.server.SimpleHTTPRequestHandler)
httpd.socket = ssl.wrap_socket (httpd.socket, certfile='./cert.pem', server_side=True)
httpd.serve_forever()

