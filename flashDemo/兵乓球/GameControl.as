package
{
	import flash.display.*;
	import flash.events.*;
	import flash.utils.getTimer;
	
	import flash.utils.Timer;
	import flash.events.TimerEvent;
	
	import flash.text.TextField;
	import flash.geom.Rectangle;
	
	public class GameControl extends MovieClip
	{
		private var wallTop:Number=10;
		private var wallBottom:Number=390;
		private var wallLeft:Number=10;
		private var wallRight:Number=510;
		private var boardY:Number=380;
		private var boardWidth:Number=60;
		private var ballSpeed:Number=3;
		
		
		private var board:Board;
		private var ball:Ball;
		private var bricks:Array;
		//小球速度
		private var ballDX:Number;
		private var ballDY:Number;
		
		private var lastTime:uint;//动画计时器
		private var moveTimer:Timer;
		
		private var ballNum:Number;//小球数量
		
		public function GameControl()
		{
			
		}
		public function startGame():void
		{
			board=new Board();
			board.y=boardY;
			addChild(board);
			
			makeBricks();
			
			newball();
			ballNum=10;
			
			addEventListener(Event.ENTER_FRAME,moveObjects);
		}
		
		public function makeBricks():void
		{
			bricks=new Array();
		
			for(var i:uint=0;i<7;++i)
			{
				for(var j:uint=0;j<5;++j)
				{
					var newBrick:Brick=new Brick();
					newBrick.x=70*i+35;
					newBrick.y=20*j+50;
					addChild(newBrick);
					bricks.push(newBrick);
				}
			}
		}
		
		public function newball():void
		{
			ball=new Ball();
			ball.x=(wallRight-wallLeft)/2+wallLeft;
			ball.y=200;
			addChild(ball);
			ballDX=3;
			ballDY=ballSpeed;
			ballNum--;
		}
		private function moveObjects(event:Event)
		{
			moveBoard();
			moveBall();
			checkBrick();
		}
		private function moveBoard():void
		{
			var newX:Number=Math.min(wallRight-boardWidth,Math.max(wallLeft,mouseX))
			board.x=newX;
		}
		private function moveBall():void
		{
			if(ball.y>=wallBottom)
			{
				 endGame();
				 return;
			}
			else if(ball.hitTestObject(board))
			{
				ballDY=-ballDY;
			}
			else if(ball.y<wallTop)
			{
				ball.y+=2*(wallTop-ball.y);
				ballDY*=-1;
			}
			else if(ball.x<wallLeft)
			{
				ball.x+=2*(wallLeft-ball.x);
				ballDX*=-1;
			}
			else if(ball.x>wallRight)
			{
				ball.x+=2*(wallRight-ball.x);
				ballDX*=-1;
			}
			ball.x+=ballDX;
			ball.y+=ballDY;
			
			
		}
		
		private function checkBrick():void
		{
			for(var i:int=bricks.length-1;i>=0;--i)
			{
					if(bricks[i].hitTestObject(ball))
					{
						ballDY*=-1;
						removeChild(bricks[i]);
						bricks.splice(i,1);
						break;
					}
			}
		}
		
		
		private function endGame():void
		{
			removeChild(ball);
			removeChild(board);
			//moveTimer.stop();
			removeEventListener(Event.ENTER_FRAME,moveObjects);
			trace("OVER");
			gotoAndStop("gameOver");
			return;
		}
	}
}