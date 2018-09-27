#include <pgmspace.h>
char ir_js[] PROGMEM = R"=====(
//window.addEventListener('load', setup);

function submitIR(name, val) {
  var xhttp = new XMLHttpRequest();
  xhttp.open('GET', 'ir_rest?' + name + '=' + val, true);
  xhttp.send();
}

function doOn() {
  submitIR("NEC", 0xFFB04F); //FIXME?
}

function doOff() {
  submitIR("NEC", 0xFF28D7); //FIXME
}



//function setup(){
//  var xhttp = new XMLHttpRequest();
//  xhttp.onreadystatechange = function() {
//    if (xhttp.readyState == 4 && xhttp.status == 200) {
      //document.getElementById('mode').innerHTML = xhttp.responseText;
      //elems = document.querySelectorAll('ul li a'); // adds listener also to existing s and b buttons
      //[].forEach.call(elems, function(el) {
      //  el.addEventListener('touchstart', handle_M_B_S, false);
      //  el.addEventListener('click', handle_M_B_S, false);
      //});
//    }
//  };
  //xhttp.open('GET', 'modes', true);
  //xhttp.send();
//}
)=====";
