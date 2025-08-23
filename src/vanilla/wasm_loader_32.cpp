#include <emscripten.h>
#include <emscripten/bind.h>
#include "../../include/vanilla/avx.hpp"
#include "../../include/vanilla/defs.hpp"
#include <emscripten/em_types.h>
#include <emscripten/val.h>

int main(){

EM_ASM({

setTimeout(function(){
document.querySelector('#splash2').style.zIndex=3000;
document.querySelector('#splash2').style.display='none';
},4200);
setTimeout(function(){
document.querySelector('#splash1').style.zIndex=3000;
document.querySelector('#splash1').style.display='none';
},4500);

setTimeout(function(){
document.querySelector('#vsiz').innerHTML=parseInt(window.innerHeight,10);
document.querySelector('#circle').width=window.innerWidth;
document.querySelector('#circle').height=window.innerHeight;
},500);

setTimeout(function(){
window.scrollTo({
top: 0,
left: 0,
behavior: "smooth",
});
},750);

var modu=document.querySelector('#modulePath').innerHTML;
const xhr=new XMLHttpRequest();
xhr.open('GET', modu, true); // Replace with your filename
xhr.responseType='arraybuffer'; // Get raw binary data
console.log('got loader run');
  
function decodeUTF32(uint8Array, isLittleEndian=true) {
const dataView=new DataView(uint8Array.buffer);
let result="";
for (let i=0; i < uint8Array.length; i += 4) {
let codePoint;
if (isLittleEndian) {
codePoint=dataView.getUint32(i, true);
} else {
codePoint=dataView.getUint32(i, false);
}
result += String.fromCodePoint(codePoint);
}
return result;
}
  
xhr.onload=function() {
console.log('got loader load');
if (xhr.status === 200) {
const utf32Data=xhr.response;
const jsCode=decodeUTF32(new Uint8Array(utf32Data), true);
const scr=document.createElement('script');
// scr.type='module';
scr.text=jsCode;
document.body.appendChild(scr);
setTimeout(function(){
var ModuleA=lib1ink();
ModuleA.onRuntimeInitialized=function(){
console.log('call mod main');
ModuleA.callMain();

/*
const mountPointInA = "/video2"; // This path will be created in Module A's FS
const rootPathInB = "/"; // Which part of Module B's FS to expose. '/' is its root.
ModuleA.FS.mkdirTree(mountPointInA);
ModuleA.FS.mount(ModuleA.FS.PROXYFS, {
root: rootPathInB,   // The directory from Module B's FS to mount
fs: FS               // Module B's actual FS object
}, mountPointInA);
console.log('[Module B JS]: Successfully mounted Module B\'s FS root at \'' + mountPointInA + '\' in Module A\'s FS.');
*/
                          
};
},2000);
}
};
  
xhr.send();


document.querySelector('#startBtnPM').addEventListener('click',function(){

function decodeUTF32(uint8Array, isLittleEndian=true) {
const dataView=new DataView(uint8Array.buffer);
let result="";
for (let i=0; i < uint8Array.length; i += 4) {
let codePoint;
if (isLittleEndian) {
codePoint=dataView.getUint32(i, true);
} else {
codePoint=dataView.getUint32(i, false);
}
result += String.fromCodePoint(codePoint);
}
return result;
}

var modu=document.querySelector('#PMmodulePath').innerHTML;
const xhr=new XMLHttpRequest();
xhr.open('GET', modu, true); // Replace with your filename
xhr.responseType='arraybuffer'; // Get raw binary data
console.log('got pm run');
  
xhr.onload=function() {
console.log('got pm load');
if (xhr.status === 200) {
const utf32Data=xhr.response;
const jsCode=decodeUTF32(new Uint8Array(utf32Data), true);
const scr=document.createElement('script');
// scr.type='module';
scr.text=jsCode;
document.body.appendChild(scr);
setTimeout(function(){
var ModuleB=createModule();
ModuleB.onRuntimeInitialized=function(){
console.log('call pm init');
ModuleB.init();
};
},2000);
}
};
  
xhr.send();

});
  
  
const vsiz=document.querySelector('#vsiz');
let menuSz=parseInt(window.innerWidth*.75,10);
let infoSz=parseInt(window.innerHeight*.25,10);

function normalResSetup(){
setTimeout(function(){
document.querySelector('#shut').innerHTML=2;
document.querySelector('#pmhig').innerHTML=parseInt(window.innerHeight,10);
document.querySelector('#ihig').innerHTML=parseInt(window.innerHeight,10);
document.querySelector('#scanvas').height=parseInt(window.innerHeight,10);
document.querySelector('#bcanvas').height=parseInt(window.innerHeight,10);
document.querySelector('#scanvas').width=parseInt(window.innerHeight,10);
document.querySelector('#bcanvas').width=parseInt(window.innerHeight,10);
document.querySelector('#di').click();
},250);
document.querySelector('#status').style.backgroundColor="green";
}
document.querySelector('#status').height=20;
document.querySelector('#status').width=parseInt(window.innerHeight,10);
const infoBtn=document.querySelector('#infoBtn');
const srs=document.querySelector('#srsiz');
const tem=document.querySelector('#tim');
const ban=document.querySelector('#menuBtn');
const sfr=document.querySelector('#slideframe');
const sfr2=document.querySelector('#slideframe2');
var $lt=tem.innerHTML;
let $sr=srs.innerHTML;
function grab$lt(){
$lt=Math.round($lt);
}
$lt=tem.innerHTML;
grab$lt();
const slo=new Slideout({
'panel':document.querySelector('#panel'),
'menu':document.querySelector('#menu'),
'padding':menuSz,
'tolerance':70,
'easing':'cubic-bezier(.32,2,.55,.27)'
});
ban.addEventListener('click',function(){
slo.toggle();
sfr.innerHTML="";
sfr2.innerHTML="";
setTimeout(function(){
grab$lt();
slt=$lt/1000;
slt=Math.round(slt);
sfr.innerHTML='<input type='+'"te'+'xt" id'+'="time'+'slider"/'+'>';
sfr2.innerHTML='<input type='+'"te'+'xt" id'+'="time'+'slider2"/'+'>';
const tsl=new rSlider({target:'#timeslider',values:{min:0.025,max:2.00},
step:[0.005],labels:false,tooltip:true,scale:false,});
const srsl=new rSlider({target:'#timeslider2',values:{min:0.05,max:2.00},
step:[0.05],labels:false,tooltip:true,scale:false,});
grab$lt();
slt=($lt/1000);
slt=slt*100;
slt=Math.round(slt);
slt=slt/100;
tsl.setValues(1.0);
srsl.setValues(1.0);
vsiz.innerHTML=Math.round(window.innerHeight);
document.querySelector('#menu').addEventListener('click',function(){
$ll=tsl.getValue();
$ll=$ll*100;
$ll=Math.round($ll);
$ll=$ll/100;
$ll=($ll*1000);
tem.innerHTML=$ll;
var sr=Math.round(srsl.getValue()*1000);
srs.innerHTML=sr;
vsiz.innerHTML=Math.round($ll/1000*window.innerHeight);
});
setTimeout(function(){
slt=tem.innerHTML;
},8);
},16);
});
const infoSlideout=new Slideout({
panel: document.querySelector('#info'),
menu: document.querySelector('#info'),
padding: infoSz,
tolerance: 70,
side: 'top',
easing: 'cubic-bezier(.32,2,.55,.27)' 
});
infoBtn.addEventListener('click', function() {
infoSlideout.toggle();
});
normalResSetup();
});
return 0;
}
