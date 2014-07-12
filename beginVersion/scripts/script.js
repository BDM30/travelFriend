(function () {
  var map;
  var masCoords = [];
  var masWays = [];
  var beginCoord;
  var masRelations = [];

  window.onload = function () {
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

  function download(requestType, adress) {
    var xml = new XMLHttpRequest();

    xml.onreadystatechange = function () {
      var i = 0;
      var coordsPolygon;
      var dataCities;

      if (xml.readyState == 4 && xml.status == 200) {
        dataCities = JSON.parse(xml.responseText);

        while (dataCities.elements[i] != null) {
          if (dataCities.elements[i].type == 'node') masCoords.push(dataCities.elements[i]);
          else if (dataCities.elements[i].type == 'way') masWays.push(dataCities.elements[i]);
          else masRelations.push(dataCities.elements[i]);
          ++i;
        }

        masCoords.sort(comparator);
        masWays.sort(comparator);

        for (var i = 0; i < masRelations.length; ++i) {
          if (i != 3 && i != 4 && i != 15) {
            var polygonCoords = getPolygonCoords(masRelations[i]);
            createPolygon(polygonCoords);
          }
        }
      }
    }

    xml.open(requestType, adress);
    xml.send(null);
  };

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