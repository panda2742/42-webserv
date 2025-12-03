<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Document</title>
</head>
<body>
	<p><?php echo "PHP CGI working\n";?></p>
	<p><?php echo "Request method: " . $_SERVER["REQUEST_METHOD"];?></p>
	<p><?php echo "Query string: " . $_SERVER["QUERY_STRING"];?></p>
	<p><?php echo "Script filename: " . $_SERVER["SCRIPT_FILENAME"];?></p>
</body>
</html>