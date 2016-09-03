package 
{
	import flash.display.*;
	
	import flash.events.*;
	
	import flash.net.*;
	
	import flash.events.*;
	import flash.utils.Timer;
	import flash.text.TextField;
	
	import flash.system.System;
	import fl.data.DataProvider;
	
	System.useCodePage=true;
	
	public class songLrc extends MovieClip
	{
		private var req1:URLRequest;
		private var lrcLoader:URLLoader;
		public var LRCarray:Array;
		private var lrcList:XML;
		
		private var mainPoint:main;
		static var index:int=1;
		private var file:String;
		public var flag:int=0;
		
		public function songLrc()
		{
			lrcList= new XML();
			lrcLoader= new URLLoader();
			//var lrcReq:URLRequest = new URLRequest("lrclist.xml");
			lrcLoader.load(new URLRequest("lrclist.xml"));
			lrcLoader.addEventListener(Event.COMPLETE, completeLrc);
		}
		function completeLrc(evt:Event):void
		{
			lrcList = XML(evt.target.data);
			loadLrc(1);
		}
		
		/*歌词*/
		public function loadLrc(i:int):void
		{
			//index=i;
			//trace(lrcList.lrc[1].@data);
			file=String(lrcList.lrc[i]);//"lrc/遥远的她.lrc"
    		req1=new URLRequest(file);
    		var loader:URLLoader=new URLLoader();
    		loader.load(req1);
    		loader.addEventListener(Event.COMPLETE,LoadFinish);
		}
		function LoadFinish(evt:Event):void
		{
    		LRCarray=new Array();
    		var list:String=evt.target.data;
    		var listarray:Array=list.split("\n");
    		var reg:RegExp=/\[[0-5][0-9]:[0-5][0-9].[0-9][0-9]\]/g;
    		for (var i=0; i<listarray.length; i++)
    		{
        		var info:String=listarray[i];//读出第i句歌词

        		var len:int=info.match(reg).length;

        		var timeAry:Array=info.match(reg);
        		var lyric:String=info.substr(len*10);

        		for (var k:int=0; k<timeAry.length; k++)
        		{
           			var obj:Object=new Object();
            		var ctime:String=timeAry[k];
            		var ntime:Number=Number(ctime.substr(1,2))*60+Number(ctime.substr(4,5));
            		obj.timer=ntime*1000;
            		obj.lyric=lyric;
					
            		LRCarray.push(obj);
        		}
    		}
    		LRCarray.sort(compare);
		}
		function compare(paraA:Object,paraB:Object):int
		{
   	 		if (paraA.timer>paraB.timer)
   	 		{
        		return 1;
    		}
    		if (paraA.timer<paraB.timer)
    		{
       	 		return -1;
   	 		}
    		return 0;
		}
	}
}