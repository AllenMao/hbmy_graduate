package
{
	import flash.display.MovieClip;
	import flash.utils.Timer;
	import flash.events.TimerEvent;
	
	import flash.events.Event;
	
	public class AirPlane extends MovieClip
	{
		private var pContainer:MovieClip;
		
		private var planeLeftX:Number=-50;
		private var planeRightX:Number=500;
		
		private var speed:Number;
		private var planeY:Number;
		
		private var moveTimer:Timer;
		
		public function AirPlane(pContainer:MovieClip)
		{
			this.pContainer=pContainer;
			init();
		}
		
		private function init():void
		{
			speed=3+Math.random()*3;//随机生成速度
			
			if(Math.random()<0.5)
			{
				this.scaleX=-1;
				this.x=planeLeftX;
			}
			else
			{
				this.scaleX=1;
				this.x=planeRightX;
				speed=-speed;
			}
			this.y=10+50*Math.random();
			//this.gotoAndStop(Math.floor(1+5*Math.random()));
			pContainer.addChild(this);
			
			moveTimer=new Timer(30,0);//单位以毫秒计算即一秒运行一次,0为运行无限次，5为5次
			moveTimer.addEventListener(TimerEvent.TIMER,timerIng)
			moveTimer.start();
		}
		
		public function destroy():void
		{
			moveTimer.stop();
			this.dispatchEvent(new Event("destroy"));//发送事件"destroy"
			pContainer.removeChild(this);//从父容器中清除
		}
		public function hitDestroy():void
		{
			moveTimer.stop();
			this.dispatchEvent(new Event("destroy"));
			pContainer.removeChild(this);//还未设计爆炸效果
			//this.gotoAndPlay();   //跳到爆炸帧
		}
		
		public function timerIng(e:TimerEvent):void
		{
			this.x+=speed;
			
			if(speed<0&&this.x<planeLeftX) destroy();
			if(speed>0&&this.x>planeRightX)  destroy();//从左向右，过界
		}
	}
}
