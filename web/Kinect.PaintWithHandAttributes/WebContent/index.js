
ws = null;
context = null;
canvas = null;
canvasHeight= null;
cavnasWidth = null;
cwd2=null;
chd2=null;
THRESHOLD = 0.1;
SPEED_THRESHOLD = 10;

rhand=[0,0,0];
lhand=[0,0,0];
rh_points=[rhand];
lh_points=[lhand];

function initialize ()
{
	ws = new WebSocket("ws://localhost:9007/");
	ws.onclose = function() { console.log("close"); };
	ws.onmessage = function(m) { process(JSON.parse(m.data)); };
	ws.onopen = function() { console.log("open"); };

	canvas = $('#canvas')[0];
	context = canvas.getContext('2d');
	canvasWidth = 500;
	canvasHeight = 500;
	cwd2 = canvasWidth/2.0;
	chd2 = canvasHeight/2.0;
}

$(document).ready(initialize);

function convertToPoint(points, hand)
{
	var c = hand.c;
	if (c < 0.6)
		return null;
	
	var lastHand = points[points.length-1];
	
	var x = hand.p[0] / 4.0;
	var y = hand.p[1] / 4.0;
	var s = hand.s;
	
	x = -x;
	y = -y;
	
	if (x < -cwd2) x = -cwd2;
	if (x > cwd2) x = cwd2;
	if (y < -chd2) y = -chd2;
	if (y > chd2) y = chd2;
	
	var cdx = x+cwd2;
	var cdy = y+chd2;

	var dx = cdx - lastHand[0];
	var dy = cdy - lastHand[1];
	console.log("hand moved " + (dx*dx + dy*dy));
	if ((dx*dx + dy*dy) > (SPEED_THRESHOLD * SPEED_THRESHOLD))
		s = lastHand[2];

	return [cdx, cdy, s];
}

function process(data)
{
	// iterate the users
	for (var i=0; i<data.length; ++i)
	{
		$('#debug').text(JSON.stringify(data[i].rhand.s) +"|"+ JSON.stringify(data[i].lhand.s));
		
		rhand = convertToPoint(rh_points, data[i].rhand);
		if (rhand != null)
			rh_points.push(rhand);
		if (rh_points.length > 100)
			rh_points.shift();
		
		lhand = convertToPoint(lh_points, data[i].lhand);
		if (lhand != null)
			lh_points.push(lhand);
		if (lh_points.length > 100)
			lh_points.shift();
	}
	
	render();
}

function drawHand (hand)
{
	if (hand == null)
		return;
	context.beginPath();
    context.arc(hand[0], hand[1], 10, 0, 2 * Math.PI, false);
	context.fillStyle = 'red';
	if (hand[2] > THRESHOLD)
		context.fill();
    context.lineWidth = 1;
    context.strokeStyle = 'white';
    context.stroke();
}

function renderPoints (points, color)
{	
    context.lineWidth = 5;

    var i=0, drawing=false;
	for(;i<points.length;++i)
	{
		context.beginPath();
		for (;i<points.length; ++i)
		{
			var p = points[i];
			if (p[2]<THRESHOLD)
			{
				drawing = false;
				break;
			}
			
			if (drawing)
				context.lineTo(p[0], p[1]);
			
			context.moveTo(p[0], p[1]);
			drawing = true;
		}		
		context.strokeStyle = color;
		context.stroke();
	}
	
	drawHand(rhand);
	drawHand(lhand);
}

function render ()
{
	context.clearRect(0,0,canvas.width, canvas.height);
	renderPoints(rh_points, "red");
	renderPoints(lh_points, "green");
}