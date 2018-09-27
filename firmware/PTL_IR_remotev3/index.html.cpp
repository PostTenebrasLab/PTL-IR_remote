#include <pgmspace.h>
char index_html[] = R"=====(
<!DOCTYPE html>
<html lang='en'>
<head>
  <meta http-equiv='Content-Type' content='text/html; charset=utf-8' />
  <meta name='viewport' content='width=device-width' />

  <title>LemanMake workshop by PTL WS2812FX Ctrl</title>

  <script type='text/javascript' src='main.js'></script>

  <style>
  * {
    font-family:sans-serif;
    margin:0;
    padding:0;
  }

  body {
    width:100%;
    max-width:675px;
    background-color:#202020;
  }
  
  h1 {
    width:65%;
    margin:25px 0 25px 25%;
    color:#454545;
    text-align:center;
  }
  
  #colorbar {
    float:left;
  }
  
  #controls {
    width:65%;
    display:inline-block;
    padding-left:5px;
  }

  ul {
    text-align:center;
  }

  ul#mode li {
    display:block;
    color: white;
  }

  ul#brightness li, ul#speed li, ul#auto li {
    display:inline-block;
    width:30%;
  }

  ul li a {
    display:block;
    margin:3px;
    padding:10px 5px;
    border:2px solid #454545;
    border-radius:5px;
    color:#454545;
    font-weight:bold;
    text-decoration:none;
  }

  ul li a.active {
    border:2px solid #909090;
    color:#909090;
  }
  </style>
</head>
<body>
  <h1>LemanMake workshop PTL main page</h1>

  <h3 style="color:white">Fonctionnalities</h3>
  <br></br>
  <ul>
    <li><a href="/led" style="color:white">led RGB control (ws2813b)</a></li> 
    <li><a href="/ir" style="color:white">infra-red led control</a></li>  
    <li><a href="/update" style="color:white">firmware update</a></li>
  </ul>
  <br></br>

  <h3 style="color:white">Measured values</h3>
  <br></br>
  <ul style="text-align:left">
    <li style="color:white" id='temperature'>Temperature : TEMP</li>
    <li style="color:white" id='humidity'>Humidity    : HUM</li>
    <li style="color:white" id='luminosity'>Luminosity  : LUM</li>
  </ul>
</body>
</html>
)=====";
