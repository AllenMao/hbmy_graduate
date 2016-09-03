package 
{
	import flash.display.*;
	import flash.events.*;
	import flash.net.URLRequest;
	import flash.geom.*;
	
	public class Image extends MovieClip 
	{
		var loader:Loader;
		var request:URLRequest;
		var image:Bitmap
		public function Image(){;}
		
		public function Display(piclocation:String) 
		{
			if (image != null)
			{
				removeBitmap();
			}
			loadBitmap(piclocation);
		}
		// 生成加载器并加载图片
		public function loadBitmap(bitmapFile:String) 
		{
			loader = new Loader();
			loader.contentLoaderInfo.addEventListener(Event.COMPLETE, loadingDone);
			request = new URLRequest(bitmapFile);
			loader.load(request);
		}
		//图片加载完毕，将图片显示出来
		private function loadingDone(event:Event):void 
		{
			// 处理加载好的文件，注意这个地方使用的loader的content属性，相当于bitmapdata
			image = Bitmap(event.target.loader.content);
			image.alpha=0.4;
			image.x=(500-image.width)/2;
			image.y=(300-image.height)/2;
			
			addChild(image);
		}
		//清理工作
		public function removeBitmap()
		{
			removeChild(image);
			image = null;
			request = null;
			loader = null;
		}
	}
}
