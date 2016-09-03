package
{
	import flash.display.*;
	import flash.events.*;
	import flash.net.URLRequest;
	import flash.geom.*;
	
	public class BitmapExample extends MovieClip
	{
		public function BitmapExample()
		{
			loadBitmap("image1.jpg");
		}
		
		public function loadBitmap(bitmapField:String):void
		{
			var loader:Loader=new Loader();
			loader.contentLoaderInfo.addEventListener(Event.COMPLETE,loadingDone);//对图片进行后期处理
			
			var Request:URLRequest=new URLRequest(bitmapField);
			loader.load(Request);
		}
		
		private function loadingDone(e:Event):void
		{
			var image:Bitmap=Bitmap(e.target.loader.content);//获得图片数据
			
			var pieceWidth:Number=image.width/6;
			var pieceHeight:Number=image.height/4;
			
			for(var x:uint=0;x<6;++x)
			{
				for(var y:uint=0;y<4;++y)
				{
					var newPuzzlePieceBitmap:Bitmap=new Bitmap(new BitmapData(pieceWidth,pieceHeight));
					
					//copyPixels带有3个参数：复制对象，复制范围，目标起始点
					newPuzzlePieceBitmap.bitmapData.copyPixels(image.bitmapData,
															   new Rectangle(x*pieceWidth,y*pieceHeight,pieceWidth,pieceHeight),
																			new Point(0,0));
					
					var newPuzzlePiece:Sprite=new Sprite();
					newPuzzlePiece.addChild(newPuzzlePieceBitmap);
					
					newPuzzlePiece.x=x*(pieceWidth+5)+20;
					newPuzzlePiece.y=y*(pieceHeight+5)+20;
					
					addChild(newPuzzlePiece);
				}
			}
		}
	}
}