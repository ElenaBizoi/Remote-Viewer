class Image
{
	constructor()
	{
		this.width = 0;
		this.height = 0;
		this.bytesPerPixel = 1;
		this.data = new ArrayBuffer(0);
	}

	isEmpty()
	{
		return this.data.byteLength == 0;
	}
	
	isOk()
	{
		return (this.data.byteLength == this.width * this.height * this.bytesPerPixel);
	}

	load(headerFile, binaryFile, handleSuccess, handleError)
	{
		var reader = new FileReader();

		reader.onloadend = function() {
			var headerLines = reader.result.split("\n");
			this.width = Number(headerLines[0]);
			this.height = Number(headerLines[1]);
			this.bytesPerPixel = Number(headerLines[2]);

			if (!Number.isInteger(this.width) || !Number.isInteger(this.height) || !Number.isInteger(this.bytesPerPixel))
			{
				handleError('Invalid value in header file ' + headerFile.name);	
				return;	
			}
			reader.onloadend = function() {
				this.data = reader.result;
				if (this.isOk())
					handleSuccess(this);	
				else
					handleError('The loaded image is corrupt. Check the input files: ' + headerFile.name + ' and ' + binaryFile.name);
			}.bind(this); 
			reader.readAsArrayBuffer(binaryFile);
		}.bind(this);
		reader.readAsText(headerFile);
	}

	paintTo(canvas)
	{	
		canvas.width = this.width;
		canvas.height = this.height;
		var context = canvas.getContext('2d');

		var pixelArray;
		if (this.bytesPerPixel == 1 || this.bytesPerPixel == 3)
			pixelArray = new Uint8Array(this.data);
		else if (this.bytesPerPixel == 2)
			pixelArray = new Uint16Array(this.data);
		else if (this.bytesPerPixel == 4)
			pixelArray = new Uint32Array(this.data);
		else return;

		const maxGrayValue = Math.pow(2, this.bytesPerPixel*8) - 1;

		var imageBuffer = context.createImageData(this.width, this.height);

		//read the pixels from the pixel array and write them into the image buffer
		var readPos = 0;
		for (let writePos = 0; writePos < imageBuffer.data.length; writePos += 4)
		{
			let gray;
			if(this.bytesPerPixel == 3)
			{
				gray = pixelArray[readPos] + (pixelArray[readPos+1] << 8) + (pixelArray[readPos+2] << 16);
				readPos += 3;
			}
			else
			{
				gray = pixelArray[readPos];
				readPos += 1;
			}
		
			gray = Math.round(gray * 255 / maxGrayValue);

			//write data of a pixel into the buffer (RGBA)
			imageBuffer.data[writePos] = gray;   //red
			imageBuffer.data[writePos+1] = gray; //green
			imageBuffer.data[writePos+2] = gray; //blue
			imageBuffer.data[writePos+3] = 255;  //alpha (opacity) 
		}
		//display the image from buffer to canvas
		context.putImageData(imageBuffer, 0, 0);
	}
}

