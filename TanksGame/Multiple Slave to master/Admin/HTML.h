#ifndef HTML_H
#define HTML_H

String html = R"(
  <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-A-Compatible" content="ie=edge">
    )";

String html2 = R"(
    <title>Document</title>
    <style>
        .button{
          color: white; 
          background-color: green; 
          width : 130px; 
          height: 50px; 
          border: 3px solid black; 
          font-size: 20px;
        }
        p,span,h1
        {
          font-family: sans-serif;

        }

       .tab1 {
          display: inline-block;
          margin-left: 60px;
        }

        .tab2 
        {
          display: inline-block;
          margin-left: 60px;
        }
        
        .square 
        {
          width: 200px;
          height: 200px;
          text-align: center;
          padding: 20px;
          border-radius: 10px;
          margin: 10px;
          color: white;
          font-family: Arial, sans-serif;
        }
        h1,h2
        {
            text-align: center;
            margin: 30px 0px;
        }

        #TeamBlock
        {
            border: 3px solid blue;
            width: 358px;
            margin-left: 36%;
        }

        #TeamA
        {
            width:160px;
            margin-left: 45px;
            display: inline-block;
            margin-top: 10px;
            margin-bottom: 15px;
        }

        #TeamB
        {
            margin-left: 15px;
            display: inline-block;
            width:130px;
            margin-top: 10px;
            margin-bottom: 15px;
        }

        #ScoreA
        {
            border-right: 3px solid rgb(246, 89, 72);
            padding-right:40px;
            margin-left: 31px;
            margin-top: 50px;
        }

        #ScoreB
        {
            margin-left: 20px;
            margin-top: 10px;
        }
    </style>
</head>
)";


String teamPage = R"(
<body>
    <h1>Team Information</h1>
    <form action='/teamData' method='post'>

        <label style="font-size: 20px;" for="time">Enter Game Time</label>
        <div>
            <input type="text" name="time" required>
        </div>
        <br>

        <label style="font-size: 20px;" for="teamNum">Enter the total number of Teams</label>
        <div>
            <input type="text" name="teamNum" required>
        </div>
        <br>

        <label style="font-size: 20px;" for="tankNum">Enter the total number of Players per Team</label>
        <div>
            <input type="text" name="tankNum" required>
        </div>
        <br>

        <label style="font-size: 20px;" for="totalScore">Enter the total Score of the game</label>
        <div>
            <input type="text" name="totalScore" required>
        </div>
        <br>

        <label style="font-size: 20px;" for="targetNum">Enter the total number of Targets per Player</label>
        <div>
            <input type="text" name="targetNum" required>
        </div>
        <br>

        <div  id="submitButton">
            <input class="button" style="background-color:  black;" type="submit" value="Send">
        </div>
        
    </form>
</body>
</html>
)";



String tankPage0 = 
R"(
<body>
    <h1>Tank Information</h1>
    <form action='/tankData' method='post'>
)";

String tankPage1 = 
R"(
    <label style="font-size: 20px;" for="tankName11">Enter the Name of tank  )";  

String tankPagetank1 = R"( (Team 1)</label>
    <div>
        <input type="text" name="tankName)";

String tankPage11 = 
R"(1" required>
    </div>
    <br>

    <label style="font-size: 20px;" for="tankScore11">Enter the Score of tank  )";  

String tankPagetank2 = R"( (Team 1)</label>
    <div>
        <input type="text" name="tankScore)";  

String tankPage12 = R"(1" required>
    </div>
    <br>
    
    <label style="font-size: 20px;" for="tankName12">Enter the Name of tank  )";  

String tankPagetank3 = R"( (Team 2)</label>
    <div>
        <input type="text" name="tankName)";  

String tankPage13 = R"(2" required>
    </div>
    <br>

    <label style="font-size: 20px;" for="tankScore12">Enter the Score of tank  )";  

String tankPagetank4 = R"( (Team 2)</label>
    <div>
        <input type="text" name="tankScore)";  

String tankPage14 = R"(2" required>
    </div>
    <br>
)";

String tankPage2 =
R"(
        <div  id="submitButton">
            <input class="button" style="background-color:  black;" type="submit" value="Send">
        </div>
    </form>
</body>
</html>
)";

String startPage = R"(

<body>
    <h1>Start Game</h1>
    <p>Press the button to start the game</p>

    <form action='/start' method='post'>
        <div  id="submitButton">
            <input class="button" style="background-color:  green;" type="submit" value="Start">
        </div>
        
    </form>
</body>

</html>

)";



String htmlHeaderResult = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Tanks Game Score</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            background: linear-gradient(135deg, #34495e, #2c3e50);
            color: #fff;
            display: flex;
            justify-content: center;
            align-items: flex-start;
            padding-top: 20px;
            min-height: 100vh;
        }
        .container {
            width: 95%;
            max-width: 500px;
            padding: 20px;
            box-sizing: border-box;
            background-color: rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            box-shadow: 0 8px 16px rgba(0, 0, 0, 0.3);
        }
        h1 {
            margin: 0;
            padding: 10px;
            font-size: 2em;
            text-align: center;
            color: #ecf0f1;
        }
        h2 {
            font-size: 1.5em;
            margin: 20px 0;
            text-align: center;
            color: #ecf0f1;
        }
        .team {
            margin: 20px 0;
            padding: 20px;
            background: rgba(255, 255, 255, 0.1);
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
        }
        .team h3 {
            font-size: 1.4em;
            color: #f39c12;
            margin-bottom: 10px;
            text-align: center;
        }
        .player {
            margin: 10px 0;
            padding: 10px;
            background: rgba(255, 255, 255, 0.2);
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
            text-align: left;
            display: flex;
            justify-content: space-between;
            padding: 15px;
            font-size: 1.1em;
        }
        .player span {
            display: inline-block;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Tanks Game Score</h1>
)";

String htmlFooterResult = R"(
    </div>
</body>
</html>
)";

#endif