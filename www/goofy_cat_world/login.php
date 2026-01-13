<?php
session_start();

// Simple hardcoded credentials for testing
$username = "admin";
$password = "meow123";

$error = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    if ($_POST['user'] === $username && $_POST['pass'] === $password) {
        $_SESSION['loggedin'] = true;
        header("Location: index.php");
        exit;
    } else {
        $error = "Invalid credentials, human.";
    }
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Cat Sentry - Login</title>
</head>
<body style="font-family: sans-serif; text-align: center; background: #f0f0f0;">
    <h1>Access Restricted by Cats</h1>
    <img src="https://cataas.com/cat/says/Stop!" alt="Stop Cat" height="280">

    <form method="post" style="margin-top: 20px;">
        <input type="text" name="user" placeholder="Username (admin)" required><br><br>
        <input type="password" name="pass" placeholder="Password (meow123)" required><br><br>
        <button type="submit">Let me in</button>
    </form>

    <?php if($error) echo "<p style='color:red;'>$error</p>"; ?>
    <br>
    <img src="https://cataas.com/cat/funny" alt="Random Cat">
</body>
</html>