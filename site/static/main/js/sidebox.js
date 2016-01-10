$(function(){
	$("li#slright span").mouseover(function() {
		$("li#slleft").css({
			height: "26px",
			backgroundImage: "none"
		});
		$("li#slleft div").css("display", "none");
		$("li#slleft span").css("fontWeight", "normal");
		$("li#slright").css({
			height: "425px",
			backgroundImage: "url(__STATICURL__/main/pics/mp-tab-2.png)"
		});
		$("li#slright span").css("fontWeight", "bold");
		$("li#slright div").css("display", "block");
	});
	$("li#slleft span").mouseover(function() {
		$("li#slright").css({
			height: "26px",
			backgroundImage: "none"
		});
		$("li#slright div").css("display", "none");
		$("li#slright span").css("fontWeight", "normal");
		$("li#slleft").css({
			height: "425px",
			backgroundImage: "url(__STATICURL__/main/pics/mp-tab-1.png)"
		});
		$("li#slleft span").css("fontWeight", "bold");
		$("li#slleft div").css("display", "block");
	});
});
