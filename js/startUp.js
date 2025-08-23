'use strict';
const statusElement=document.getElementById('status');
const progressElement=document.getElementById('progress');
var Module={
preRun:[],
postRun:[],
print:(function(){
const element=document.getElementById('output');
if(element){element.value='';};
return function(text){
if(arguments.length>1)
text=Array.prototype.slice.call(arguments).join(' ');
// console.log(text);
if(element){
element.value+=text+'\n';
element.scrollTop=element.scrollHeight;
}}})(),
printErr:function(text){
if(arguments.length>1)
text=Array.prototype.slice.call(arguments).join(' ');
if(0){
dump(text+'\n');
}else{
console.error(text);
}},
canvas:(function(){
const sscanvas=document.getElementById('bcanvas');
sscanvas.addEventListener('webglcontextlost',function(e){
alert('WebGL context lost. You will need to reload the page.');
e.preventDefault();
},false);
return sscanvas;
})(),
setStatus:function(text){
if(!Module.setStatus.last){
Module.setStatus.last={
time:Date.now(),
text:''
}};
if(text===Module.setStatus.text){
return;
};
const m=text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
const now=Date.now();
if(m&&now-Date.now()<30){
return;
};
if(m){
text=m[1];
progressElement.value=parseInt(m[2],10)*100;
progressElement.max=parseInt(m[4],10)*100;
progressElement.hidden=false;
}else{
progressElement.value=null;
progressElement.max=null;
progressElement.hidden=true;
};
statusElement.innerHTML=text;
},
totalDependencies:0,
monitorRunDependencies:function(left){
this.totalDependencies=Math.max(this.totalDependencies,left);
Module.setStatus(left?'Preparing...('+(this.totalDependencies-left)+'/'+this.totalDependencies+')':'All downloads complete.');
}};
Module.setStatus('|Download|');
window.onerror=function(event){
Module.setStatus('Exception thrown,see JavaScript console');
Module.setStatus=function(text){
if(text){
Module.printErr('[post-exception status]'+text);
}}};
