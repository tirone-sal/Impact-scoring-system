
String html = R"(
  <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-A-Compatible" content="ie=edge">
    )";

String refreshTag = R"(<meta http-equiv="refresh" content="5">)";

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

        <label style="font-size: 20px;" for="tankNum">Enter the total number of Tanks</label>
        <div>
            <input type="text" name="tankNum" required>
        </div>
        <br>

        <label style="font-size: 20px;" for="team1Name">Enter Team 1 Name</label>
        <div>
            <input type="text" name="team1Name" required>
        </div>
        <br>

        <label style="font-size: 20px;" for="team2Name">Enter Team 2 Name</label>
        <div>
            <input type="text" name="team2Name" required>
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

String scoreHead = R"(
<body>
    <h1>Tanks Game</h1>

    <h2>Game Type</h2>

    <form action='/start' method='post'>
      <div  id="submitButton">
          <input class="button" style="background-color:  green;" type="submit" value="Fetch Score">
      </div>
    </form>

    <h2>Game Time: 
)";

