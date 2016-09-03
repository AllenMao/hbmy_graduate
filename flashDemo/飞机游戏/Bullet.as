package
{
	import flash.display.MovieClip;
	import flash.utils.Timer;
	import flash.events.TimerEvent;
	
	import flash.events.Event;
	
	public class Bullet extends MovieClip
	{
		private var pContainer:MovieClip;
		private var initX:Number;
		private var initY:Number;
		
		private var moveTimer:Timer;
		private var speed:Number=-6;
		
		private var topY:Number=-30;
		
		public function Bullet(pContainer:MovieClip,initX:Number,initY:Number)
		{
			this.pContainer=pContainer;
			this.initX=initX;
			this.initY=initY;
			init();
		}
		private function init():void
		{
			this.x=initX;
			this.y=initY;
			pContainer.addChild(this);
			
			moveTimer=new Timer(30,0);//单位以毫秒计算即一秒运行一次,0为运行无限次，5为5次
			moveTimer.addEventListener(TimerEvent.TIMER,timerIng)
			moveTimer.start();
		}
		
		private function timerIng(e:TimerEvent):void
		{
			this.y+=speed;
			if(this.y<=topY) destroy();
		}
		
		public function destroy():void
		{
			moveTimer.stop();
			this.dispatchEvent(new Event("destroy"));
			pContainer.removeChild(this);
		}
	}
}