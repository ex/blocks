
package com.ex.stc;

import android.view.GestureDetector.OnDoubleTapListener;
import android.view.GestureDetector.OnGestureListener;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnKeyListener;

public class GameControls implements OnKeyListener, OnGestureListener, OnDoubleTapListener {
	private GameActivity gameActivity;
	
	/**
	 * The usable screen width
	 */
	private int width;
	private int moveWidth;
	private int height;
	private int moveHeight;

	public GameControls(GameActivity gameActivity, int width, int height) {
		this.gameActivity = gameActivity;
		this.width = width;
		this.height = height;
		this.moveWidth = width / (2 * GameActivity.BOARD_WIDTH);
		this.moveHeight = height / (2 * GameActivity.BOARD_HEIGHT);
	}

	/* (non-Javadoc)
	 * @see android.view.GestureDetector.OnGestureListener#onDown(android.view.MotionEvent)
	 */
	@Override
	public boolean onDown(MotionEvent e) {
		return false;
	}

	/* (non-Javadoc)
	 * @see android.view.GestureDetector.OnGestureListener#onFling(android.view.MotionEvent, android.view.MotionEvent, float, float)
	 */
	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX,
			float velocityY) {
		/* uncomming to handle drops with flings TODO: make this an option
		if (fling && velocityY > 40 && Math.abs(e1.getY(e1.getActionIndex()) - e2.getY(e2.getActionIndex())) > moveHeight * 4) {
			gameActivity.drop();
			return true;
		}*/
		return false;
	}

	/* (non-Javadoc)
	 * @see android.view.GestureDetector.OnGestureListener#onLongPress(android.view.MotionEvent)
	 */
	@Override
	public void onLongPress(MotionEvent e) {
		// TODO Auto-generated method stub

	}
	
	private float lastMoveXLocation = 0;
	private float startScrollXLocation = 0;
	private float lastMoveYLocation = 0;
	private float startScrollYLocation = 0;
	
	/* (non-Javadoc)
	 * @see android.view.GestureDetector.OnGestureListener#onScroll(android.view.MotionEvent, android.view.MotionEvent, float, float)
	 */
	@Override
	public boolean onScroll(MotionEvent start, MotionEvent cur, float distanceX,
			float distanceY) {
		if (startScrollXLocation != start.getX(start.getActionIndex())) {
			startScrollXLocation = start.getX(start.getActionIndex());
			lastMoveXLocation = startScrollXLocation;
			startScrollYLocation = start.getY(start.getActionIndex());
			lastMoveYLocation = startScrollYLocation;
		}
		
		float curX = cur.getX(cur.getActionIndex());
		if (curX > lastMoveXLocation + moveWidth) {
			lastMoveXLocation += moveWidth;
			gameActivity.moveRightStart();
			gameActivity.moveRightEnd();
		}
		else if (curX < lastMoveXLocation - moveWidth) {
			lastMoveXLocation -= moveWidth;
			gameActivity.moveLeftStart();
			gameActivity.moveLeftEnd();
		}
		
		float curY = cur.getY(cur.getActionIndex());
		if (curY > lastMoveYLocation + moveHeight) {
			lastMoveYLocation += moveHeight;
			gameActivity.moveDownStart();
			gameActivity.moveDownEnd();
		}		
		return true;
	}

	/* (non-Javadoc)
	 * @see android.view.GestureDetector.OnGestureListener#onShowPress(android.view.MotionEvent)
	 */
	@Override
	public void onShowPress(MotionEvent e) {
		// TODO Auto-generated method stub

	}

	/* (non-Javadoc)
	 * @see android.view.GestureDetector.OnGestureListener#onSingleTapUp(android.view.MotionEvent)
	 */
	@Override
	public boolean onSingleTapUp(MotionEvent event) {
		return false; // let #onSingleTapConfirmed() handle it 
	}

	@Override
	public boolean onKey(View v, int keyCode, KeyEvent event) {
		switch (event.getAction()) {
		case KeyEvent.ACTION_DOWN:
			switch (keyCode) {
			case KeyEvent.KEYCODE_DPAD_DOWN:
				gameActivity.moveDownStart();
				return true;
			case KeyEvent.KEYCODE_DPAD_UP:
				gameActivity.rotateCWStart();
				return true;
			case KeyEvent.KEYCODE_DPAD_LEFT:
				gameActivity.moveLeftStart();
				return true;
			case KeyEvent.KEYCODE_DPAD_RIGHT:
				gameActivity.moveRightStart();
				return true;
			case KeyEvent.KEYCODE_DPAD_CENTER:
				gameActivity.drop();
				return true;
			}
			break;
		case KeyEvent.ACTION_UP:
			switch (keyCode) {
			case KeyEvent.KEYCODE_DPAD_DOWN:
				gameActivity.moveDownEnd();
				return true;
			case KeyEvent.KEYCODE_DPAD_LEFT:
				gameActivity.moveLeftEnd();
				return true;
			case KeyEvent.KEYCODE_DPAD_RIGHT:
				gameActivity.moveRightEnd();
				return true;
			case KeyEvent.KEYCODE_DPAD_UP:
				gameActivity.rotateCWEnd();
				return true;
			}
		}
		
		return false;
	}

	/**
	 * On double tap, drop
	 */
	@Override
	public boolean onDoubleTap(MotionEvent e) {
		gameActivity.drop();
		return true;
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent e) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent event) {
		float x = event.getX(event.getActionIndex());
		if (x * 2 > width) { // check if on right or left side of screen.
			gameActivity.rotateCWStart();
			gameActivity.rotateCWEnd();
			return true;
		}
		gameActivity.rotateCCWStart();
		gameActivity.rotateCCWEnd();
		return true;
	}

}
