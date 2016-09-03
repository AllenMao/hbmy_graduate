package
{
	import flash.display.MovieClip;
	import flash.display.SimpleButton;
	import flash.text.TextField;
	import flash.events.KeyboardEvent;
	
	import flash.utils.Timer;
	import flash.events.TimerEvent;
	
	import flash.events.Event;
	
	public class GameControl extends MovieClip
	{
		private var scoreNum:int;//得分
		private var bulletNum:int;//子弹剩余数量
		private var bulletTotal:int=30;//子弹总数
		
		private var ScoreTxt:TextField;//成绩文本
		private var BulletTxt:TextField;//表示剩余子弹数文本
		
		private var aaGun:AAGun;//坦克的对象
		private var airPlaneArr:Array;//管理敌机的数组
		private var bulletArr:Array;//管理子弹的数组
		
		private var checkTimer:Timer;//检查子弹和敌机的定时器
		private var planeTimer:Timer;//不定时产生敌机的定时器
		
		
		public function GameControl()
		{//原因应该就是getChildByName返回的类型是DisplayObject,要用as转化
			ScoreTxt=this.getChildByName("scoreTxt") as TextField;//ScoreTxt不能与实例名称一样
			BulletTxt=this.getChildByName("bulletTxt") as TextField;
			
			startGame()//开始游戏
		}
		public function startGame():void
		{
			// 初始化子弹数和分数
			scoreNum=0;
			bulletNum=bulletTotal;
			showScore();
			
			aaGun=new AAGun(this);//生成坦克 直接添加到父容器
			airPlaneArr=new Array();
			bulletArr=new Array();
			
			checkTimer=new Timer(30,0);
			checkTimer.addEventListener(TimerEvent.TIMER,checkIng);
			checkTimer.start();
			
			stage.addEventListener(KeyboardEvent.KEY_DOWN,keyDownFun);
			stage.addEventListener(KeyboardEvent.KEY_UP,keyUpFun);
			
			setNextPlane();//设置下一辆飞机
		}
		/*
		
			设置下一辆飞机
		
		*/
		private function setNextPlane():void
		{
			planeTimer=new Timer(1000+Math.random()*1000,1);
			planeTimer.addEventListener(TimerEvent.TIMER_COMPLETE,newPlane);
			planeTimer.start();
		}
		
		private function newPlane(e:TimerEvent):void
		{
			var plane:AirPlane=new AirPlane(this);
			plane.addEventListener("destroy",planeDestroy);
			airPlaneArr.push(plane);
			setNextPlane();
		}
		
		
		/*
			键盘控制
		*/
		private function keyDownFun(e:KeyboardEvent):void
		{
			switch(e.keyCode)
			{
				case 37:
					aaGun.leftMove();
					break;
				case 39:
					aaGun.rightMove();
					break;
				case 32:
					fireBullet();
					break;
			}
		}
		
		private function keyUpFun(e:KeyboardEvent):void
		{
			switch(e.keyCode)
			{
				case 37:
				case 39:
					aaGun.stopMove();
					break;
				default:
				
			}
		}
		
		
		
		/*
			生成子弹
		*/
		private function fireBullet():void
		{
			
			if(bulletNum==0) return;
			
			var b:Bullet=new Bullet(this,aaGun.x+25,aaGun.y-10);
			
			b.addEventListener("destroy",bulletDestroy);
			bulletArr.push(b);
			bulletNum--;
			showScore();
		}
		/*
			检测碰撞情况
		
		*/
		private function checkIng(e:TimerEvent):void
		{//为什么从最大开始检索
			for(var i:int=airPlaneArr.length-1;i>=0;--i)
			{
				for(var j:int=bulletArr.length-1;j>=0;--j)
				{
					if(airPlaneArr[i].hitTestObject(bulletArr[j]))
					{
						airPlaneArr[i].hitDestroy();
						bulletArr[j].destroy();
						scoreNum++;
						showScore();
						break;
					}
				}
			}
			
			if(bulletNum==0&&bulletArr.length==0)
			{
				endGame();
			}
		}
		private function endGame():void
		{
				checkTimer.stop();
				checkTimer=null;
				planeTimer.stop();
				planeTimer=null;
				for(var i:int=airPlaneArr.length-1;i>=0;--i)
				{
					airPlaneArr[i].destroy();
				}
				airPlaneArr=null;
				aaGun.destroy();
				aaGun=null;
				stage.removeEventListener(KeyboardEvent.KEY_DOWN,keyDownFun);
				stage.removeEventListener(KeyboardEvent.KEY_UP,keyUpFun);
				gotoAndStop("gameOver");
		}
		
		
		/*事件处理*/
		private function planeDestroy(e:Event):void
		{
			for(var i in airPlaneArr)
			{
				if(airPlaneArr[i]==(e.target as AirPlane))
				{
					airPlaneArr.splice(i,1);//移除第i个元素
					break;
				}
			}
		}
		
		private function bulletDestroy(e:Event):void
		{
			for(var i in bulletArr)
			{
				if(bulletArr[i]==(e.target as Bullet))
				{
					bulletArr.splice(i,1);//移除第i个元素
					break;
				}
			}
		}
		private function showScore():void
		{
			ScoreTxt.text=String(scoreNum);
			BulletTxt.text=String(bulletNum);
		}
	}
}