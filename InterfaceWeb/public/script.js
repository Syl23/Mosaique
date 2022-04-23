const Http = new XMLHttpRequest();
const url = 'http://127.0.0.1:8080';

function execute(){

	if($('#entree').get(0).files.length > 0){
		let split = $('#split').val();
		let scale = $('#scale').val();
		var myFormData = new FormData();
		myFormData.append('imageFile', $('#entree').get(0).files[0]);
		
		$.ajax({
		  url: url + '/Execute?split=' + split + '&scale=' + scale,
		  type: 'POST',
		  processData: false, // important
		  contentType: false, // important
		  dataType : 'json',
		  data: myFormData
		})
		.always(function(){
			window.open(url + "/image/sortie.ppm", '_blank').focus();
		});
	}
}
