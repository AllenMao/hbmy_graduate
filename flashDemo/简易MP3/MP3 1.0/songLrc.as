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
	
	public class songLrc extends MovieClip
	{
		private var xml:XML;
		private var url:URLRequest;
		private var loader:URLLoader;
		
		public var timer:Timer;
		
		private var lrcArr:Array;
		private var playingId:uint;
		private var totalsTime:Number;
		
		
		public var lrctxt:TextField;
		private var mainPoint:main;
		public var i:Number = 0;
		
		public function songLrc()
		{
			System.useCodePage=true;//关键啊啊啊啊啊啊
			lrctxt=new TextField();
			xml=new XML();
			lrcArr= new Array();
			
		}
		public function loadLrc(lrclocation:String,totalsTime:Number,point:main)
		{
				i=0;
				this.totalsTime=totalsTime;
				mainPoint=point;
   				loader=new URLLoader(new URLRequest(lrclocation));
   				loader.addEventListener(Event.COMPLETE, xmlLoaded);
		}
		
		private function xmlLoaded(event:Event):void 
		{
				xml=XML(loader.data);
			
				//my_Array[i] = myXML.name[i];
				timer=new Timer(10000,0);
				timer.addEventListener(TimerEvent.TIMER,timeIng);
				timer.start();
			//trace(xml.name.@a+"/n");
		}
		private function timeIng(e:TimerEvent)
		{
			
				lrctxt.text=xml.name[i];
				
				//trace(lrctxt.text);
			
			mainPoint.lrcTxt.text=xml.name[i];
			i++;
			if(i==xml.name.length()) timer.stop();
		}
		
	}
}