package
{
	import flash.display.MovieClip;
	import flash.utils.Timer;
	import flash.events.TimerEvent;
	
	public class AAGun extends MovieClip
	{
		private var pContainer:MovieClip;
		
		private var gunInitX:Number=240;
		private var gunInitY:Number=300;
		private var gunMinX:Number=10;
		private var gunMaxX:Number=470;
		
		private var speed:Number=3;
		
		private var moveTimer:Timer;
		
		private var sideStr:String;
		
		
		public function AAGun(pContainer:MovieClip)
		{
			this.pContainer=pContainer;
			
			init();
		}
		
		private function init():void
		{
			this.x=gunInitX;
			this.y=gunInitY;
			pContainer.addChild(this);
			
			sideStr="stop";
			moveTimer=new Timer(30,0);//单位以毫秒计算即一秒运行一次,0为运行无限次，5为5次
			moveTimer.addEventListener(TimerEvent.TIMER,timerIng)
			moveTimer.start();
		}
		
		public function leftMove():void
		{
			sideStr="left";
		}
		
		public function rightMove():void
		{
			sideStr="right";
		}
		
		public function stopMove():void
		{
			sideStr="stop";
		}
		
		public function destroy():void
		{
			moveTimer.stop();
			pContainer.removeChild(this);
		}
		
		public function timerIng(e:TimerEvent):void
		{
			switch(sideStr)
			{
				case "left":
					this.x-=speed;
					break
				case "right":
					this.x+=speed;
					break;
				case "stop":
					return;
			}
			if(this.x<=gunMinX)
			{
				this.x=gunMinX;
			}
			if(this.x>=gunMaxX)
			{
				this.x=gunMaxX;
			}
		}
	}
}