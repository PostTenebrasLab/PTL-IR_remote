#include <pgmspace.h>
char ir_html[] = R"=====(
<!DOCTYPE html>
<html lang='en'>
<head>
  <meta http-equiv='Content-Type' content='text/html; charset=utf-8' />
  <meta name='viewport' content='width=device-width' />
  <meta http-equiv="refresh" content="20">
  
  <title>LemanMake workshop PTL IR Ctrl</title>

  <script type='text/javascript' src='ir.js'></script>

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
  <h1>LemanMake workshop PTL infra-rouge remote Control</h1>


</body>
</html>
)=====";

