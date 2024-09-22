#ifndef INFOPAGE_H
#define INFOPAGE_H

String htmlHeader = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Information Page</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            height: 100vh;
            background: linear-gradient(135deg, #2c3e50, #4ca1af);
            color: #2c3e50;
        }
        .container {
            width: 100%;
            height: 100vh;
            overflow-y: auto;
            padding: 20px;
            background: rgba(255, 255, 255, 0.9);
            border-radius: 12px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
            box-sizing: border-box;
            margin: 0 auto;
        }
        h1 {
            text-align: center;
            margin: 0;
            color: #2c3e50;
        }
        .form-block {
            width: 100%;
            max-width: 500px;
            margin: 20px auto;
            padding: 20px;
            background: #ecf0f1;
            border: 1px solid #bdc3c7;
            border-radius: 8px;
            box-sizing: border-box;
        }
        .form-block h2 {
            margin-top: 0;
            color: #2c3e50;
        }
        .form-block input {
            display: block;
            width: calc(100% - 22px);
            margin: 10px 0;
            padding: 10px;
            font-size: 16px;
            border: 1px solid #bdc3c7;
            border-radius: 4px;
            box-sizing: border-box;
        }
        .form-block input:focus {
            border-color: #7f8c8d;
            outline: none;
        }
        .submit-button {
            display: block;
            margin: 20px auto;
            padding: 15px 30px;
            font-size: 18px;
            color: #fff;
            background-color: #27ae60;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            text-align: center;
            max-width: 200px;
        }
        .submit-button:hover {
            background-color: #2ecc71;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Information Page</h1>
        <form action="/tankData" method="post">
)";

String htmlFooter = R"(
            <button type="submit" class="submit-button">Submit</button>
        </form>
    </div>
</body>
</html>
)";


String teamFormStart = R"(
            <div class="form-block">
                <h2>Team )";
String teamFormMid = R"(</h2>
                <input type="text" name="team)";
String teamFormEnd = R"(Name" placeholder="Team )";
String teamFormEnd2 = R"(" required>
)";

String playerFormStart = R"(
                <input type="text" name="player)";
String playerFormMid = R"(" placeholder="Player )";
String playerFormEnd = R"(" required>
)";

String targetInputStart = R"(
                <input type="number" name="target)";
String targetInputMid = R"(Score" placeholder="Enter Target )";
String targetInputEnd = R"( Score to be minus" required>
)";

#endif