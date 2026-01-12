#!/usr/bin/env python3
import os
import sys
from datetime import datetime

# CGI Header
print("Content-Type: text/html\r")
print("\r")

# HTML Response
print("""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI Test - Python</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 1000px;
            margin: 50px auto;
            padding: 20px;
            background-color: #f5f5f5;
        }
        h1 {
            color: #333;
            border-bottom: 3px solid #4CAF50;
            padding-bottom: 10px;
        }
        h2 {
            color: #555;
            margin-top: 30px;
        }
        .info-box {
            background-color: white;
            padding: 20px;
            margin: 20px 0;
            border-radius: 5px;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1);
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 10px;
        }
        th, td {
            padding: 10px;
            text-align: left;
            border-bottom: 1px solid #ddd;
        }
        th {
            background-color: #4CAF50;
            color: white;
        }
        tr:hover {
            background-color: #f5f5f5;
        }
        .success {
            color: #4CAF50;
            font-weight: bold;
        }
        .back-link {
            display: inline-block;
            margin-top: 20px;
            padding: 10px 20px;
            background-color: #4CAF50;
            color: white;
            text-decoration: none;
            border-radius: 5px;
        }
        .back-link:hover {
            background-color: #45a049;
        }
    </style>
</head>
<body>
    <h1>🐍 CGI Python - Test Réussi!</h1>

    <div class="info-box">
        <p class="success">✓ Le script CGI fonctionne correctement!</p>
        <p><strong>Date/Heure:</strong> """)
print(datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
print("""</p>
        <p><strong>Python Version:</strong> """)
print(sys.version)
print("""</p>
    </div>

    <div class="info-box">
        <h2>Variables d'environnement CGI</h2>
        <table>
            <tr>
                <th>Variable</th>
                <th>Valeur</th>
            </tr>""")

# Display important CGI environment variables
cgi_vars = [
    'REQUEST_METHOD',
    'QUERY_STRING',
    'CONTENT_TYPE',
    'CONTENT_LENGTH',
    'SERVER_SOFTWARE',
    'SERVER_NAME',
    'SERVER_PORT',
    'SERVER_PROTOCOL',
    'SCRIPT_NAME',
    'PATH_INFO',
    'REMOTE_ADDR',
    'HTTP_USER_AGENT',
    'HTTP_ACCEPT',
    'HTTP_HOST'
]

for var in cgi_vars:
    value = os.environ.get(var, '<em>non défini</em>')
    print(f"""
            <tr>
                <td><strong>{var}</strong></td>
                <td>{value}</td>
            </tr>""")

print("""
        </table>
    </div>

    <div class="info-box">
        <h2>Toutes les variables d'environnement</h2>
        <table>
            <tr>
                <th>Variable</th>
                <th>Valeur</th>
            </tr>""")

# Display all environment variables
for key, value in sorted(os.environ.items()):
    if key not in cgi_vars:  # Skip already displayed ones
        print(f"""
            <tr>
                <td><strong>{key}</strong></td>
                <td>{value}</td>
            </tr>""")

print("""
        </table>
    </div>

    <a href="/cgi-test.html" class="back-link">← Retour au formulaire</a>
</body>
</html>""")
