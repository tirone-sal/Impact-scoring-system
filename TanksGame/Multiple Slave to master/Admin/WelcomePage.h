#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

String page1 = R"(
  <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Impact Scoring System</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f4f4f9;
        }
        h1 {
            color: #333;
            margin-bottom: 20px;
        }
        .button-container {
            text-align: center;
        }
        .button-container button {
            padding: 15px 30px;
            font-size: 16px;
            color: #fff;
            background-color: #007bff;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.3s;
        }
        .button-container button:hover {
            background-color: #0056b3;
        }
    </style>
</head>
<body>
    <h1>Welcome to Impact Scoring System</h1>
    <div class="button-container">
        <form action="/new" method="GET">
            <button type="submit">Create New Game</button>
        </form>
    </div>
</body>
</html>
)";

#endif