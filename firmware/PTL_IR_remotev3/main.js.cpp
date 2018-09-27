#include <pgmspace.h>
char main_js[] PROGMEM = R"=====(
window.addEventListener('load', setup);
var refreshId = window.setInterval(setup, 2000);
window.addEventListener

function stopInterval(){
  window.clearInterval(refreshId);
}

function submitVal(name, val) {
  var xhttp = new XMLHttpRequest();
  xhttp.open('GET', 'set?' + name + '=' + val, true);
  xhttp.send();
}

function getMousePos(can, evt) {
  r = can.getBoundingClientRect();
  return {
    x: evt.clientX - r.left,
    y: evt.clientY - r.top
  };
}

function getTemperature(){
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
     document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open('GET', 'temp', true);
  xhttp.send();
}

function getHumidity(){
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
     document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open('GET', 'hum', true);
  xhttp.send();
}

function getLuminosity(){
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
     document.getElementById("luminosity").innerHTML = this.responseText;
    }
  };
  xhttp.open('GET', 'lum', true);
  xhttp.send();
}

function setup(){
  try{
  getTemperature();
  getHumidity();
  getLuminosity();
  }
  catch(e) {
    stopInterval();
  }
}

)=====";
