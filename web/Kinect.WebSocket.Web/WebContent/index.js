
ws = new WebSocket("ws://localhost:9007/");

ws.onclose = function() { console.log("close"); }
ws.onmessage = function(m) { console.log(m.data); $('#debug').text(m.data); }
ws.onopen = function() { console.log("open"); }

