package
{
	import flash.display.*;//fl.controls.Button
	import flash.events.*;
	import flash.net.URLRequest;
	import flash.geom.*;
	import flash.utils.Timer;
	
	public class GameControl extends MovieClip
	{
		//小块之间的间隔和偏移量
		private var pieceSpace:Number=2;
		private var xMove:Number=175;
		private var yMove:Number=25;
		
		//小块数量
		private var numPieceX:int=4;
		private var numPieceY:int=3;
		
		//随机移动次数
		private var numMoveRand:int=200;
		
		//动画的次数和时间
		private var slideSteps:Number=10;
		private var slideTime:Number=250;
		
		//小块尺寸
		private var pieceWidth:Number;
		private var pieceHeight:Number;
		
		//小快集合
		private var pieceImageObjects:Array;
		
		//跟踪移动
		private var blankPoint:Point;
		private var slidingPiece:Object;
		private var slideDirection:Point;
		private var slideAnimation:Timer=null;
		
		private var ScoreTxt:TextField;//成绩文本
		private var steps:Number;
		
		public function GameControl()
		{
			startGame();
		}
		public function startGame()
		{
			steps=1000;
			ScoreTxt=this.getChildByName("score") as TextField;
			showScore();
			blankPoint=new Point(numPieceX-1,numPieceY-1);
			loadBitmap("image3.jpg");
		}
		
		public function loadBitmap(bitmapField:String):void
		{
			var loader:Loader=new Loader();
			loader.contentLoaderInfo.addEventListener(Event.COMPLETE,loadingDone);//对图片进行后期处理
			
			var Request:URLRequest=new URLRequest(bitmapField);
			loader.load(Request);
			//addChild(loader);
		}
		
		public function loadingDone(e:Event):void
		{
			var image:Bitmap=Bitmap(e.target.loader.content);//获得图片数据
			pieceWidth=image.width/numPieceX;
			pieceHeight=image.height/numPieceY;
			
			makeImagePieces(image.bitmapData);//切成小块
			
			moveImagePieces();//移动小块
			
		}
		
		public function makeImagePieces(bitmapData:BitmapData)
		{
			pieceImageObjects=new Array();
			
			for(var x:uint=0;x<numPieceX;++x)
			{
				for(var y:uint=0;y<numPieceY;++y)
				{
					if(blankPoint.equals(new Point(x,y))) continue;
					
					var newPuzzlePieceBitmap:Bitmap=new Bitmap(new BitmapData(pieceWidth,pieceHeight));
					
					//copyPixels带有3个参数：复制对象，复制范围，目标起始点
					newPuzzlePieceBitmap.bitmapData.copyPixels(bitmapData,
											new Rectangle(x*pieceWidth,y*pieceHeight,pieceWidth,pieceHeight),
											new Point(0,0));
					
					var newPuzzlePiece:Sprite=new Sprite();
					newPuzzlePiece.addChild(newPuzzlePieceBitmap);
					
					newPuzzlePiece.x=x*(pieceWidth+pieceSpace)+xMove;
					newPuzzlePiece.y=y*(pieceHeight+pieceSpace)+yMove;
					
					addChild(newPuzzlePiece);
					
					//创建对象，存储在数组中
					var newImageObject:Object=new Object();
					newImageObject.currentLoc=new Point(x,y);//当前位置
					newImageObject.homeLoc=new Point(x,y);//原始位置
					newImageObject.piece=newPuzzlePiece;
					
					newImageObject.piece.addEventListener(MouseEvent.CLICK,clickImagePiece);
					
					pieceImageObjects.push(newImageObject);
				}
			}
			
		}
		/*随机移动*/
		public function moveImagePieces()
		{
			for(var i:int=0;i<numMoveRand;++i)
				piecesRandom();
		}
		public function piecesRandom()
		{
			var vaildPieceObjects:Array=new Array();
			
			for(var i:uint=0;i<pieceImageObjects.length;++i)
			{
				if(vaildMove(pieceImageObjects[i])!="none")
				{
					//trace("haha");
					vaildPieceObjects.push(pieceImageObjects[i]);
				}
			}
			
			
			
			//挑选一个随机移动
			var pick:uint=Math.floor(Math.random()*vaildPieceObjects.length);
			movePiece(vaildPieceObjects[pick],false);
		}
		
		
		
		
		public function vaildMove(pieceObject:Object):String
		{
			if((pieceObject.currentLoc.x==blankPoint.x)&&(pieceObject.currentLoc.y==blankPoint.y+1))
				return "up";
				
			if((pieceObject.currentLoc.x==blankPoint.x)&&(pieceObject.currentLoc.y==blankPoint.y-1))
				return "down";
				
			if((pieceObject.currentLoc.x==blankPoint.x+1)&&(pieceObject.currentLoc.y==blankPoint.y))
				return "left";
			
			if((pieceObject.currentLoc.x==blankPoint.x-1)&&(pieceObject.currentLoc.y==blankPoint.y))
				return "right";
			
			return "none"
		}
		
		/*拼图小块被单击*/
		public function clickImagePiece(e:MouseEvent)
		{
			for(var i:uint=0;i<pieceImageObjects.length;++i)//找出被单击的小块并移动它
			{
				if(pieceImageObjects[i].piece==e.currentTarget)
				{
					movePiece(pieceImageObjects[i],true);
					break;
				}
			}
		}
		/*得到小块移动到空白区域*/
		public function movePiece(pieceObject:Object,slideEffect:Boolean)
		{
			switch(vaildMove(pieceObject))
			{
				case"up":
					movePieceInDirection(pieceObject,0,-1,slideEffect);
					break;
				case"down":
					movePieceInDirection(pieceObject,0,1,slideEffect);
					break;
				case"left":
					movePieceInDirection(pieceObject,-1,0,slideEffect);
					break;
				case"right":
					movePieceInDirection(pieceObject,1,0,slideEffect);//无语  左右不分
					break;
			}
		}
		public function movePieceInDirection(pieceObject:Object,dx,dy:int,slideEffect:Boolean)
		{
			pieceObject.currentLoc.x+=dx;
			pieceObject.currentLoc.y+=dy;
			
			blankPoint.x-=dx;
			blankPoint.y-=dy;
			if(slideEffect) 
			{
				startSlide(pieceObject,dx*(pieceWidth+pieceSpace),dy*(pieceHeight+pieceSpace));
				steps--;
				showScore();
			}
			else  //随机移动
			{
				pieceObject.piece.x=pieceObject.currentLoc.x*(pieceWidth+pieceSpace)+xMove;
				pieceObject.piece.y=pieceObject.currentLoc.y*(pieceHeight+pieceSpace)+yMove;
			}
		}
		
		//设置滑动
		public function startSlide(pieceObject:Object,dx,dy:Number)
		{
			if(slideAnimation!=null) slideDone(null);
			slidingPiece=pieceObject;
			slideDirection=new Point(dx,dy);
			 
			slideAnimation=new Timer(slideTime/slideSteps,slideSteps);
			
			slideAnimation.addEventListener(TimerEvent.TIMER,slidePiece);
			
			slideAnimation.addEventListener(TimerEvent.TIMER_COMPLETE,slideDone);
			slideAnimation.start();
		}
		
		public function slidePiece(e:Event)
		{
			slidingPiece.piece.x+=(slideDirection.x)/slideSteps;
			slidingPiece.piece.y+=(slideDirection.y)/slideSteps;
		}
		
		//完成滑动
		public function slideDone(e:Event)
		{
			slidingPiece.piece.x=slidingPiece.currentLoc.x*(pieceWidth+pieceSpace)+xMove;
			slidingPiece.piece.y=slidingPiece.currentLoc.y*(pieceHeight+pieceSpace)+yMove;//注意！！！！
			
			slideAnimation.stop();
			slideAnimation=null;
			//检测游戏是否完成
			if(pieceComplete()||steps==0)
			{
				clearPiece();
				gotoAndStop(5);
				ScoreTxt=this.getChildByName("score") as TextField;
				showScore()
			}
		}
		
		public function pieceComplete():Boolean
		{
			for(var i:uint=0;i<pieceImageObjects.length;++i)
			{
				if(!pieceImageObjects[i].currentLoc.equals(pieceImageObjects[i].homeLoc)) return false;
			}
			return true;
		}
		
		public function clearPiece()
		{
			for(var i in pieceImageObjects)
			{
				pieceImageObjects[i].piece.removeEventListener(MouseEvent.CLICK,clickImagePiece);
				removeChild(pieceImageObjects[i].piece);
			}
			pieceImageObjects=null;
		}
		
		public function showScore()
		{
			ScoreTxt.text=String(steps);
		}
	}
}