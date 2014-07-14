(function () 
{
  	var masCoords = [];
  	var masWays = [];
  	var masRelations = [];
	var map = null;
  	var msgClickedMarker = "Click to another marker to draw a line";
  	var loadingMessage = "Loading...";
  	var markerClicked = {
    	isClicked: false,
    	lastMarker: null,
    	message: null
  	};

  	window.onload = function () 
    {
    	var mapOptions = {
      		center: new google.maps.LatLng(60.0, 30.3),
      		zoom: 8,
      		mapTypeId: google.maps.MapTypeId.ROADMAP,
      		visiable: true
    	};

    	map = new google.maps.Map(document.getElementById("googleMap"), mapOptions);

    	google.maps.event.addListener(map, 'click', function (event) {
      		clickedHandler(event);
    	});
      	downloadPolygons("GET", "http://overpass-api.de/api/interpreter?data=[out:json];area[%22boundary%22=%22administrative%22][%22name%22=%22%D0%A1%D0%B0%D0%BD%D0%BA%D1%82-%D0%9F%D0%B5%D1%82%D0%B5%D1%80%D0%B1%D1%83%D1%80%D0%B3%22]-%3E.a;%28relation%28area.a%29[%22admin_level%22=%225%22];%29;out%20body;%3E;out%20skel%20qt;");
  	};

<<<<<<< HEAD
  	function clickedHandler(event) 
  	{
    	if (markerClicked.isClicked) 
        {
      		markerClicked.isClicked = false;
      		markerClicked.message.close();
    	} 	
      	else placeMarker(event.latLng);
  	};

  	function markerClickedHandler(marker) 
  	{
    	var line;
    	var place = marker.getPosition();
    
    	if (!markerClicked.isClicked) 
    	{
      		markerClicked.isClicked = true;
      		markerClicked.lastMarker = marker;
      		markerClicked.message = new google.maps.InfoWindow({
        		content: msgClickedMarker
      		});
      		markerClicked.message.open(map, marker);
    	} 
   		else if (markerClicked.lastMarker != marker) 
    	{
      		markerClicked.message.close();
      		line = placePolyline([markerClicked.lastMarker.getPosition(), place]);
      		marker.lines.push(line);
      		markerClicked.lastMarker.lines.push(line);
      		markerClicked.isClicked = false;
      		marker.links.push(markerClicked.lastMarker);
      		markerClicked.lastMarker.links.push(marker);
    	}
  	};

  	function placeMarker(place) 
  	{

    	var markerOptions = {
      		draggable: true,
      		position: place,
      		map: map
    	};

    	var marker = new google.maps.Marker(markerOptions);
    	marker.curPos = place;
    	marker.lines = [];
    	marker.links = [];
    	google.maps.event.addListener(marker, 'click', function (event) {
      		closeInfoWindow(marker);
          	markerClickedHandler(marker);
    	});
    	google.maps.event.addListener(marker, 'drag', function (event) {
      		closeInfoWindow(marker);
      		redrawLines(marker); 
    	});
    	google.maps.event.addListener(marker, 'dblclick', function (event) {
      		dblclickMarkerHandler(marker);
    	});
    	google.maps.event.addListener(map, 'click', function (event) {
      		closeInfoWindow(marker);
    	});
  	};
  
  	function closeInfoWindow(marker)
  	{
    	if ('infoWindow' in marker)
      		marker.infoWindow.close();
  	};
  
  	function dblclickMarkerHandler(marker)
  	{
    	markerClicked.isCLicked = false;
    	markerClicked.message.close();
    	if ('infoWindow' in marker)
      		marker.infoWindow.close();
	
  		placeInfoWindow(marker);
  	};

  	function redrawLines(marker) 
  	{
    	var place = marker.getPosition();
    	for (var i = 0; i < marker.lines.length; ++i) 
        {
      		var end = marker.links[i].getPosition();
      		marker.lines[i].setPath([end, place]);
    	}
  	};

  	function placePolyline(coords) 
  	{
    	var lineOptions;
    	var line;

    	lineOptions = {
      		path: coords,
      		strokeColor: "red",
      		strokeWeight: 5,
      		map: map
    	}
    	return new google.maps.Polyline(lineOptions);
  	};
  
  	function placeInfoWindow(marker) 
  	{
      	var xml = new XMLHttpRequest();
    	marker.infoWindow = new google.maps.InfoWindow({
      		content: loadingMessage
    	});
    	
    	marker.infoWindow.open(map, marker);

    	xml.open("GET", 'http://maps.googleapis.com/maps/api/geocode/json?latlng=' + marker.getPosition().lat() + ',' + marker.getPosition().lng() + '&sensor=false&language=ru');

    	xml.onreadystatechange = function () 
        {
      		var answer;
      		var answerObject;

      		if (xml.readyState == 4 && xml.status == 200)
            {
        		answer = xml.responseText;
        		answerObject = JSON.parse(answer);
        		marker.infoWindow.setContent(answerObject.results[2]["formatted_address"]);
      		}
    	};    
    	xml.send(null);
  	};
  
  	function downloadPolygons(requestType, adress) 
  	{
    	var request = new XMLHttpRequest();      	

    	request.onreadystatechange = function() 
    	{
    		var i = 0;
      		var coordsPolygon;
          	var dataRequest;

      		if (request.readyState == 4 && request.status == 200) 
      		{
        		dataRequest = JSON.parse(request.responseText);
  
        		while (dataRequest.elements[i] != null) 
            	{
          			if (dataRequest.elements[i].type == 'node') 	masCoords.push(dataRequest.elements[i++]);
          			else if (dataRequest.elements[i].type == 'way') 	   masWays.push(dataRequest.elements[i++]);
          			else masRelations.push(dataRequest.elements[i++]);
        		}

        		masCoords.sort(comparator);
        		masWays.sort(comparator);

        		for (var i = 0; i < masRelations.length; ++i)
            		createPolygons(masRelations[i]);
      		}
    	}

    	request.open(requestType, adress);
    	request.send(null);
  	};

  	function comparator(first, second) 
  	{
    	return (first.id - second.id);
  	};

  	function findById(mas, id)
  	{
    	var middle;
    	var begin = 0;
     	var end = mas.length - 1;
          
       	while (begin <= end)
        {
        	middle = ~~((begin + end) / 2);
            if (mas[middle].id == id) 
            	return mas[middle];
            begin = (id < mas[middle].id)? begin : middle + 1;
    		end = (id < mas[middle].id)? middle - 1 : end;
    	}
    	return null;
    }
  
  	function createPolygons(relation) 
  	{
    	var wayList = [];
    	var way;
      
    	for (var i = 0; i < relation.members.length; ++i)
      		if (relation.members[i].type == "way")
              wayList.push(findById(masWays, relation.members[i].ref));
      	
    	try
        {
        	while (wayList.length)
    		{
      			way = wayList.pop();
      			placePolygon(getPolygon(way, wayList));      	
    		}
        }
      	catch(error)
        {
        	alert(error)
        }
      	
  	};
  
  	function getPolygon(curWay, wayList) 
  	{
    	var polygon = curWay.nodes;
    	var scopeEnd = curWay.nodes[curWay.nodes.length - 1];
    	var nextWay;
    	var begin;
    	var end;
      	var prev;

    	while (polygon[0] != polygon[polygon.length - 1])
        {
          	prev = polygon[polygon.length - 1];
    		for (var i = 0; i < wayList.length; ++i) 
        	{
      			nextWay = wayList[i];
      			begin = nextWay.nodes[0];
      			end = nextWay.nodes[nextWay.nodes.length - 1];

      			if (scopeEnd == begin || scopeEnd == end) 
            	{
        			if (scopeEnd == end) 
                	{
          				scopeEnd = begin;
          				nextWay.nodes.reverse();
        			}	 
              		else scopeEnd = end;

        			polygon = polygon.concat(nextWay.nodes);
        			curWay = nextWay;
        			wayList.splice(i, 1);
        			break;
      			}
    		}
          	if (prev == polygon[polygon.length - 1]) 
            	throw new Error("Wrong input relation object. Impossible to find closure");
        }
    	return getCoordsPolygon(polygon);
  	};
  
  	function placePolygon(coordsPolygon) 
  	{
    	var polygonOptions = {
      		paths: coordsPolygon,
      		fillColor: "red",
     	 	map: map
    	};
    	var polygon = new google.maps.Polygon(polygonOptions);
      		google.maps.event.addListener(polygon, 'click', function (event) {
      		clickedHandler(event);
    	});
      
  	};

  	function getCoordsPolygon(polygon) 
  	{
    	var node;
    
    	for (var i = 0; i < polygon.length; ++i) 
    	{
      		node = findById(masCoords, polygon[i]);
      		polygon[i] = new google.maps.LatLng(node.lat, node.lon);
    	}
    	return polygon;
  	};
})()
=======
  function createPolygon(coordsPolygon) {
    var polygonOptions = {
      paths: coordsPolygon,
      fillColor: "red",
      map: map
    }
    var polygon = new google.maps.Polygon(polygonOptions);
  };

  function comparator(first, second) {
    if (first.id < second.id) return -1;
    if (first.id > second.id) return 1;
    return 0
  };

  function findById(massive, id) {
    var length = massive.length;
    for (var i = 0; i < length; ++i)
    if (massive[i].id == id) return massive[i];
    return -1;
  };

  function placeMarker(place) {
    var markerOptions = {
      map: map,
      position: place,
      draggable: true
    }
    var marker = new google.maps.Marker(markerOptions);

    google.maps.event.addListener(marker, 'dblclick', function () {
      placeInfoWindow(marker);
    });
    google.maps.event.addListener(marker, 'click', function (event) {
      createLine(event.latLng);
    })
  };

  function placeInfoWindow(marker) {
    var infoWindow = new google.maps.InfoWindow({
      content: '...'
    });
    var xml = new XMLHttpRequest();


    infoWindow.open(map, marker);

    xml.open("GET", 'http://maps.googleapis.com/maps/api/geocode/json?latlng=' + marker.getPosition().lat() + ',' + marker.getPosition().lng() + '&sensor=false&language=ru');

    xml.onreadystatechange = function () {
      var answer;
      var answerObject;

      if (xml.readyState == 4 && xml.status == 200) {
        answer = xml.responseText;
        answerObject = JSON.parse(answer);
        infoWindow.setContent(answerObject.results[2]["formatted_address"]);
      }
    };

    xml.send(null);
    beginCoord = null;
  };

  function createLine(coords) {
    var lineOptions;
    var line;

    if (beginCoord == null) beginCoord = coords;
    else {
      lineOptions = {
        path: [beginCoord, coords],
        strokeColor: "red",
        strokeWeight: 5,
        map: map
      }
      line = new google.maps.Polyline(lineOptions);
      beginCoord = null;
    };

  };

  function getPolygonCoords(relation) {
    var curWay = findById(masWays, relation.members[0].ref);
    var polygon = curWay.nodes;
    var endWay = curWay.nodes[curWay.nodes.length - 1];
    var nextWay;
    var node;
    var begin;
    var end;
    relation.members.splice(0, 1)

    while (relation.members.length)
    for (var j = 0; j < relation.members.length; ++j) {
      nextWay = findById(masWays, relation.members[j].ref);
      begin = nextWay.nodes[0];
      end = nextWay.nodes[nextWay.nodes.length - 1];

      if (endWay == begin || endWay == end) {
        if (endWay == end) {
          endWay = begin;
          nextWay.nodes.reverse();
        } else endWay = end;

        polygon = polygon.concat(nextWay.nodes);
        curWay = nextWay;
        relation.members.splice(j, 1);
        break;
      }
    }
    for (var z = 0; z < polygon.length; ++z) {
      node = findById(masCoords, polygon[z]);
      polygon[z] = new google.maps.LatLng(node.lat, node.lon);
    }
    return polygon;
  }
})()
>>>>>>> origin/master
