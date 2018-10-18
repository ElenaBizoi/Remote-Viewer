function init()
{
	var image = new Image();
	var transformMatrix = [0, 1, 1, 0];

	//initialize tranform matrix input fields
	for (let i = 0; i < transformMatrix.length; i++)
	{
		let input = document.getElementById('tm' + i);
		if (typeof input === 'undefined')
			continue;

		input.min = -1;
		input.max = 1;
		input.value = transformMatrix[i];
	}

	$('#open-button').click(function() {
		var headerFile = $('#header-file').prop('files')[0];
		var binaryFile = $('#binary-file').prop('files')[0];
		disableInterface();
		image.load(headerFile, binaryFile, onImageLoadSuccess, onImageLoadError);
	});

	$('#transform-button').click(function() {
		var transformMatrix = getTransformMatrix();
		transformImage(image, transformMatrix);
	});

	$('#flip-horizontal-button').click(function() {
		var horizontalFlip = [-1, 0, 0, 1];
		transformImage(image, horizontalFlip);
	});
	
	$('#flip-vertical-button').click(function() {
		var verticalFlip = [1, 0, 0, -1];
		transformImage(image, verticalFlip);
	});
	
	$('#rotate-left-button').click(function() {
		var leftRotate = [0, 1, -1, 0];
		transformImage(image, leftRotate);
	});
	
	$('#rotate-right-button').click(function() {
		var rightRotate = [0, -1, 1, 0];
		transformImage(image, rightRotate);
	});
	
	$('#header-file').change(updateOpenButton);
	$('#binary-file').change(updateOpenButton);

	window.updateTransformControls = function() {
		$('#transform-controls')[0].disabled = image.isEmpty() || !image.isOk();
	}

	enableInterface();
}

function getTransformMatrix()
{
	var matrix = [];
	for (let i = 0; i < 4; i++)
	{
		let inputField = document.getElementById('tm' + i);
		matrix[i] = Number(inputField.value);
	}
	return matrix;
}

function verifyTransformMatrix(matrix)
{
	if (matrix.length !== 4)
		return false;

	for (let i = 0; i < matrix.length; i++)
	{
		if (!Number.isInteger(matrix[i]) || matrix[i] < -1 || matrix[i] > 1)
			return false;
	}

	function xor(a, b) { return !a !== !b; }

	if (xor(matrix[0], matrix[3]) || xor(matrix[1], matrix[2]) || !xor(matrix[0], matrix[1]))
		return false;

	return true;
}

function transformImage(image, transformMatrix)
{
	if (!verifyTransformMatrix(transformMatrix))
	{
		alert("Error: The transormation matrix: [ " + transformMatrix + " ] is not valid!");
		return;
	}

	var imageBytes = new Uint8Array(image.data);
	var imageBase64 = base64js.fromByteArray(imageBytes);
	
	var transformRequest = {
		width: image.width,
		height: image.height,
		bytesPerPixel: image.bytesPerPixel,
		data: imageBase64,
		transformMatrix: transformMatrix,
	}
	disableInterface();
	
	$.ajax({
		type: 'post',
		dataType: 'json',
		contentType: 'application/json',	
		data: JSON.stringify(transformRequest), 

		success: function (response) {
			if ('error' in response)
			{
				onImageTrasnformError(response['error']);
			}
			else
			{
				image.width = response.width;
				image.height = response.height;
				image.bytesPerPixel = response.bytesPerPixel;
				let imageBytes = base64js.toByteArray(response.data);
				image.data = imageBytes.buffer;
			
				if (image.isOk())	
					onImageTransformSuccess(image);
				else
					onImageTransformError('Server returned a corrupt transformed image');
			}
			enableInterface();
		},

		error: function(jqXHR, textStatus, error) {
			onImageTransformError('Server did not respond');
			enableInterface();
		},
	});
}

function updateOpenButton()
{
	var bothFilesSelected = $('#header-file')[0].files.length && $('#binary-file')[0].files.length;
	$('#open-button')[0].disabled = !bothFilesSelected;
}

function enableInterface()
{
	updateOpenButton();
	updateTransformControls();
	$('#header-file')[0].disabled = false;
	$('#binary-file')[0].disabled = false;
}

function disableInterface()
{
	$('#open-button')[0].disabled = true;
	$('#header-file')[0].disabled = true;
	$('#binary-file')[0].disabled = true;
	$('#transform-controls')[0].disabled = true;
}

function displayImage(image)
{
	var canvas = document.getElementById('image-canvas');
	image.paintTo(canvas);	
}

function onImageLoadSuccess(image)
{
	displayImage(image);
	enableInterface();
}

function onImageLoadError(errorMsg)
{
	alert('Error: ' + errorMsg);
	enableInterface();
}

function onImageTransformSuccess(image)
{
	displayImage(image)
}

function onImageTransformError(errorMsg)
{
	alert('Error: Failed to transform the image: ' + errorMsg);
}
