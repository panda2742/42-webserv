#!/usr/bin/env python3
print("Content-Type: text/html; charset=utf-8\r\n\r\n")

print("""
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Cat Generator Ultimate</title>
    <style>
        body { font-family: sans-serif; background: #1a1a1a; color: white; text-align: center; padding: 20px; }
        .box { background: #333; padding: 20px; border-radius: 10px; display: inline-block; border: 2px solid #e67e22; }
        input, select, button { padding: 15px; font-size: 18px; margin: 10px; border-radius: 5px; border: none; }
        button { background: #e67e22; color: white; cursor: pointer; font-weight: bold; }
        img { margin-top: 20px; border: 5px solid white; max-width: 90%; border-radius: 10px; }
    </style>
</head>
<body>

    <div class="box">
        <h1>🐾 GENERATEUR DE CHATS 🐾</h1>

        <input type="text" id="le_texte" placeholder="Texte sur le chat...">

        <select id="la_categorie">
            <option value="cute">Mignon</option>
            <option value="funny">Drôle</option>
            <option value="grumpy">Grincheux</option>
            <option value="sleepy">Dort</option>
            <option value="orange">Roux (Orange)</option>
            <option value="black">Noir</option>
            <option value="white">Blanc</option>
            <option value="siamese">Siamois</option>
            <option value="ginger">Gingembre</option>
            <option value="shock">Choqué</option>
            <option value="sad">Triste</option>
        </select>

        <button onclick="update()">NEW CAT</button>
    </div>

    <div id="container">
        <br>
        <img id="chat_img" src="https://cataas.com/cat/cute" alt="Loading cat...">
    </div>

    <script>
        function update() {
            const txt = document.getElementById('le_texte').value || " ";
            const cat = document.getElementById('la_categorie').value;
            const img = document.getElementById('chat_img');

            // On force le refresh avec le timestamp ?t=
            const url = "https://cataas.com/cat/" + cat + "/says/" + encodeURIComponent(txt) + "?t=" + Date.now();
            img.src = url;
        }

        // Touche Entrée
        document.getElementById('le_texte').addEventListener('keypress', function (e) {
            if (e.key === 'Enter') update();
        });
    </script>

</body>
</html>
""")