// JavaScript Document
function refresh(){		
	//var urls = document.getElementById("url").value;
	var urls ="192.168.0.19";
	//var port = document.getElementById("port").value;
	var port = "8080";
	var url = "http://" + urls + ":" + port + "/cgi-bin/pressure-get.cgi";
	
	var oAjax = null;
	oAjax = new XMLHttpRequest();
	
	oAjax.open("GET",url,true);
	oAjax.setRequestHeader("Access-Control-Allow-Origin", "*");
	oAjax.setRequestHeader("Access-Control-Allow-Methods", "PUT,POST,GET,DELETE,OPTIONS");
	oAjax.setRequestHeader("Access-Control-Allow-Headers", "X-Requested-With,Content-Type,Accept");
	oAjax.send();
	oAjax.onreadystatechange = function () {
		if (oAjax.readyState === 4) {
			if (oAjax.status === 200) {
				//��Ӧ�ɹ�
				postMessage(oAjax.responseText);
			} else {
			   //��Ӧʧ��
			   postMessage("error��" + oAjax.responseText);
			   //alert("��Ӧʧ��,����������");
			}
		}
	};	
	
	setTimeout("refresh()", 500);
}

refresh();
