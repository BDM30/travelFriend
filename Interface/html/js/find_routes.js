	function changeTypeTravel() {
		var one = document.getElementById("sel");
		var two = document.getElementById("fra");
		if (one.value == "once") {
			fra.src="html/once_travel.html";
		}
		else {
			if (one.value == "every day/week") {
				fra.src="html/every_travel.html";
			}
		}
	}
	
	function clearInputOne() {
		alert("pressed input");
		var one = document.getElementById("inp0");
		if (one.value == "input adress") {
			one.value="";
			style="color:black";
		}
   }
   
   function clearInputTwo() {
    alert("pressed input");
	var one = document.getElementById("inp1");
	if (one.value == "input adress") {
			one.value="";
	}
   }
   
   function inputOneState() {
    alert("changed flag");
	var one = document.getElementById("ch1");
	var inp = document.getElementById("inp0");
	if (one.checked) {
		inp.disabled = true;
	}
	else
	{
		inp.disabled = false;
	}
	
   }
   
   function inputTwoState() {
    alert("changed flag");
	var one = document.getElementById("ch2");
	var inp = document.getElementById("inp1");
	if (one.checked) {
		inp.disabled = true;
	}
	else
	{
		inp.disabled = false;
	}
   }