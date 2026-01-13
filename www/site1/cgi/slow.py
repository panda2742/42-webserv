#!/usr/bin/env python3
import time

# CGI Header
print("Content-Type: text/html\r")
print("\r")

# Wait 5 seconds to simulate slow CGI
time.sleep(5)

# HTML Response
print("<!DOCTYPE html>")
print("<html><body>")
print("<h1>Slow CGI Response</h1>")
print("<p>This took 5 seconds to generate.</p>")
print("</body></html>")
