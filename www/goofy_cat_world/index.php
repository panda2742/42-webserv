<?php
session_start();

if (!isset($_SESSION['loggedin']) || $_SESSION['loggedin'] !== true) {
    header("Location: login.php");
    exit;
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Cat Headquarters</title>
</head>
<body style="font-family: Comic Sans MS, sans-serif; background: #e0ffe0; padding: 50px;">
    <h1>Welcome to the Secret Cat Society!</h1>
    <p>If you see this, your PHP CGI and Session handling are working perfectly.</p>

    <div style="display: flex; flex-wrap: wrap; gap: 10px;">
        <img src="https://cataas.com/cat/cute" alt="Cat 1">
        <img src="https://cataas.com/cat/grumpy" alt="Cat 2">
        <img src="https://cataas.com/cat/jump" alt="Cat 3">
    </div>

    <br>
    <a href="logout.php" style="padding: 10px; background: orange; color: white; text-decoration: none; border-radius: 5px;">Log Out and Feed the Cats</a>
</body>
</html>