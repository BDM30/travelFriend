(function () 
{  
  var map;
  var masCoords = [];
  var masWays = [];
  var beginCoord;

  window.onload = function () 
  {
    var mapOptions = {
      center: new google.maps.LatLng(60, 30.3),
      zoom: 8,
      mapTypeId: google.maps.MapTypeId.ROADMAP
    };

    map = new google.maps.Map(document.getElementById("googleMap"), mapOptions);

    google.maps.event.addListener(map, 'click', function (event) {
      placeMarker(event.latLng);
    });
    download("GET", "http://overpass-api.de/api/interpreter?data=[out:json];area[%22boundary%22=%22administrative%22][%22name%22=%22%D0%A1%D0%B0%D0%BD%D0%BA%D1%82-%D0%9F%D0%B5%D1%82%D0%B5%D1%80%D0%B1%D1%83%D1%80%D0%B3%22]-%3E.a;%28relation%28area.a%29[%22admin_level%22=%225%22];%29;out%20body;%3E;out%20skel%20qt;");
  };

  function download(requestType, adress) 
  {
    var xml = new XMLHttpRequest();

    xml.onreadystatechange = function () 
    {
      var i = 0;
      var coordsPolygon;
      var dataCities;

      if (xml.readyState == 4 && xml.status == 200) 
      {
        dataCities = JSON.parse(xml.responseText);

        while (dataCities.elements[i] != null) 
        {
          if (dataCities.elements[i].type == 'node') masCoords.push(dataCities.elements[i]);
          else if (dataCities.elements[i].type == 'way') masWays.push(dataCities.elements[i]);
          ++i;
        }

        masCoords.sort(comparator);
        for (var i = 0; i < masWays.length; ++i) 
        {
          coordsPolygon = getPolygonCoords(masWays[i].nodes);
          createPolygon(coordsPolygon);
        }
      }
    }

    xml.open(requestType, adress);
    xml.send(null);
  };

  function createPolygon(coordsPolygon) 
  {
    var polygonOptions = {
      path: coordsPolygon,
      stringColor: "red",
      map: map
    }
    var polygon = new google.maps.Polyline(polygonOptions);
  };

  function getPolygonCoords(massive) 
  {
    var length = massive.length;
    var polygonCoords = [];
	var point;
    
    for (var i = 0; i < length; ++i) 
    {
      point = findById(masCoords, massive[i]);
      polygonCoords.push(new google.maps.LatLng(point.lat, point.lon));
    }
    return polygonCoords;
  };

  function comparator(first, second) 
  {
    if (first.id < second.id) return -1;
    if (first.id > second.id) return 1;
    return 0
  };

  function findById(massive, id) 
  {
    var length = massive.length;
    for (var i = 0; i < length; ++i)
    if (massive[i].id == id) return massive[i];
    return -1;
  };

  function placeMarker(place) 
  {
    var markerOptions = {
      map: map,
      position: place
    }
    var marker = new google.maps.Marker(markerOptions);

    google.maps.event.addListener(marker, 'dblclick', function () {
      placeInfoWindow(marker);
    });
    google.maps.event.addListener(marker, 'click', function (event) {
      createLine(event.latLng);
    })
  };

  function placeInfoWindow(marker) 
  {
    var infoWindow = new google.maps.InfoWindow({
      content: '...'
    });
    var xml = new XMLHttpRequest();
    
    
    infoWindow.open(map, marker);

    xml.open("GET", 'http://maps.googleapis.com/maps/api/geocode/json?latlng=' + marker.getPosition().lat() + ',' + marker.getPosition().lng() + '&sensor=false&language=ru');

    xml.onreadystatechange = function () 
    {
      var answer;
      var answerObject;
      
      if (xml.readyState == 4 && xml.status == 200) 
      {
        answer = xml.responseText;
        answerObject = JSON.parse(answer);
        infoWindow.setContent(answerObject.results[2]["formatted_address"]);
      }
    };
    
    xml.send(null);
    beginCoord = null;
  };

  function createLine(coords) 
  {
    var lineOptions;
    var line;
    
    if (beginCoord == null) beginCoord = coords;
    else 
    {
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
  
})()