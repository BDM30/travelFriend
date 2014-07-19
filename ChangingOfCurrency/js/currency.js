var courseFilePath = "xml/Course_Currency_02-07-2014.xml";
var listCurrency = [];

function insertOptions(){
	for (var i = 0; i < listCurrency.length; ++i){
		$("#outputListCurrency").append(
		"<option value=" + listCurrency[i] + ">" + listCurrency[i] + "</option>");
	}
}

function xmlParser(xml){
	
	$("#load").fadeOut();
	
	$(xml).find("Valute").each(function(){
		var value = $(this).find("CharCode").text();
		listCurrency.push(value); 	
	});
	
	listCurrency.sort();
	insertOptions();
};

function findData(xml){
	
	var decimalOnly = /^\s*-?[0-9]\d*(\.\d{1,2})?\s*$/;
	
	try {
		
		var shortName = $("#outputListCurrency").val();
		var value = $("#sum").val();
		
		if (value == "") throw new Error("Empty value");
			else if (!decimalOnly.test(value)) throw new Error("value must be of decimal digits");
			else if (value < 0) throw new Error("Value isn't to be below zero");
			else if (isNaN(parseFloat(value))) throw new Error("Value isn't to be as symbols");
		
		$(xml).find("Valute").each(function(){
			if (shortName == $(this).find("CharCode").text()){
			
				var nominal = $(this).find("Nominal").text();
				var course = $(this).find("Value").text();
				var result = parseFloat(value) * parseFloat(nominal) / parseFloat(course);
			
				document.getElementById("result").value = Number(result).toFixed(2);
			}
		});
	}
	catch (e){
		alert(e);
	}
}
	
$(document).ready(function(){
    
  $.ajax({    
    type: "GET", //The method of sending for data
    url: courseFilePath, //It is reference on xml file
    dataType: "xml", //The type of data
    success: xmlParser //Calling function
  });  	
	
  $("#clear").click(function() {
    document.getElementById("sum").value = "";
    document.getElementById("result").value = "";
  });

  $("#convert").click(function() {
    var selectCurrency = $("#inputListCurrency").val();
    $.get(courseFilePath, findData, "xml");
  });
});